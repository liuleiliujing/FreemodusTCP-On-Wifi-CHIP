/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#include "syslog_port_device.h"

#ifdef MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE

#include "hal_resource_assignment.h"
#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
#include "hal_hw_semaphore.h"
#endif
#ifdef MTK_CPU_NUMBER_0
#include "offline_dump.h"
#endif

typedef struct {
    uint32_t dump_buffer_size;      /* Size of dump buffer */
    uint32_t dump_buffer_addr[2];   /* Start address of dump buffer, view with CPU 0, other CPU should do remap */
    uint32_t dump_buffer_pos[2];    /* Current write position in dump buffer, view with relative address */
    uint8_t dump_buffer_wrap[2];    /* Whether the wrap condition is detected in dump buffer */
    uint8_t dump_buffer_ready;      /* Is ready buffer exist? */
    uint8_t dump_ongoing_number;    /* The dump buffer which is written now */
    uint32_t share_buffer_addr;     /* Start address of share VFIFO buffer, view with CPU 0, other CPU should do remap */
    uint32_t share_buffer_size;     /* Size of share VFIFO buffer */
    uint32_t share_buffer_pos;      /* Current write position in share VFIFO buffer, view with relative address */
} offline_dump_share_variable_t;

#ifdef PORT_SYSLOG_SINGLE_CPU_ENV
static offline_dump_share_variable_t offline_dump_share_variable;
static volatile offline_dump_share_variable_t *g_offline_dump_share_variable = &offline_dump_share_variable;
#else
static volatile offline_dump_share_variable_t *g_offline_dump_share_variable = (volatile offline_dump_share_variable_t *)HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_DUMP_VAR_START;
#endif

/* Memory remap across CPUs (primary CPU remap to other CPU) */
#ifdef PORT_SYSLOG_SINGLE_CPU_ENV
#define memory_remap_primary_cpu_to_current_cpu(address) address
#else
#if defined(MTK_CPU_NUMBER_1)
#define memory_remap_primary_cpu_to_current_cpu(address) hal_memview_cm4_to_dsp0(address)
#elif defined(MTK_CPU_NUMBER_2)
#define memory_remap_primary_cpu_to_current_cpu(address) hal_memview_cm4_to_dsp1(address)
#else
#define memory_remap_primary_cpu_to_current_cpu(address) address
#endif
#endif

#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
#define DUMP_SEMAPHORE_TAKE() while(hal_hw_semaphore_take(HW_SEMAPHORE_SYSLOG_WRAP_LAYER) != HAL_HW_SEMAPHORE_STATUS_OK)
#define DUMP_SEMAPHORE_GIVE() while(hal_hw_semaphore_give(HW_SEMAPHORE_SYSLOG_WRAP_LAYER) != HAL_HW_SEMAPHORE_STATUS_OK)
#else
#define DUMP_SEMAPHORE_TAKE()
#define DUMP_SEMAPHORE_GIVE()
#endif

#ifdef MTK_CPU_NUMBER_0
/*
 * false: dump syslog to flash only when the user call log_trigger_write_to_flash()
 * true: always dump syslog to flash
 */
static volatile bool g_syslog_dump_is_active = false;
#endif

static void internal_copy_to_ram_buffer(uint8_t *curr_log_addr, uint32_t curr_log_size)
{
    uint32_t index, curr_ram_buf_size;
    uint8_t *src_addr, *dst_addr, *dst_start_addr;

    index = g_offline_dump_share_variable->dump_ongoing_number;
    dst_start_addr = (uint8_t *)memory_remap_primary_cpu_to_current_cpu(g_offline_dump_share_variable->dump_buffer_addr[index]);

    if ((g_offline_dump_share_variable->dump_buffer_pos[index] + curr_log_size) >= g_offline_dump_share_variable->dump_buffer_size) {
        curr_ram_buf_size = g_offline_dump_share_variable->dump_buffer_size - g_offline_dump_share_variable->dump_buffer_pos[index];
        dst_addr = dst_start_addr + g_offline_dump_share_variable->dump_buffer_pos[index];
        src_addr = curr_log_addr;
        memcpy(dst_addr, src_addr, curr_ram_buf_size);
        dst_addr = dst_start_addr;
        src_addr += curr_ram_buf_size;
        memcpy(dst_addr, src_addr, curr_log_size - curr_ram_buf_size);
        g_offline_dump_share_variable->dump_buffer_pos[index] = curr_log_size - curr_ram_buf_size;
        g_offline_dump_share_variable->dump_buffer_wrap[index] = true;
    } else {
        dst_addr = dst_start_addr + g_offline_dump_share_variable->dump_buffer_pos[index];
        src_addr = curr_log_addr;
        memcpy(dst_addr, src_addr, curr_log_size);
        g_offline_dump_share_variable->dump_buffer_pos[index] += curr_log_size;
    }
}

static void internal_dump_buffer_swap(void)
{
    if (g_offline_dump_share_variable->dump_ongoing_number) {
        g_offline_dump_share_variable->dump_ongoing_number = 0;
    } else {
        g_offline_dump_share_variable->dump_ongoing_number = 1;
    }
}

#ifdef MTK_CPU_NUMBER_0

#include "nvdm.h"
#include "exception_handler.h"

typedef struct {
    uint8_t log_to_flash;
    uint8_t always_dump;
} log_dump_config_t;

ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static uint8_t g_syslog_dump_buffer[2][OFFLINE_REGION_SYSLOG_CELL_VALID_SIZE];

/* Define the tiemout value to trigger syslog dump to flash even the ram buffer is NOT becomen full */
#define SYSLOG_OFFLINE_DUMP_TIMEOUT 120 * 1000000

static uint32_t g_ram_flash_irq_mask;

extern hal_uart_baudrate_t g_sylog_uart_baudrate;

static uint32_t internal_get_ready_dump_buffer(void)
{
    if (g_offline_dump_share_variable->dump_ongoing_number) {
        return 0;
    } else {
        return 1;
    }
}

static void internal_offline_dump_region_write(uint32_t index)
{
    uint32_t curr_flash_addr, curr_ram_addr;

    offline_dump_region_alloc(OFFLINE_REGION_SYSLOG, &curr_flash_addr);

    if (g_offline_dump_share_variable->dump_buffer_wrap[index] == true) {
        curr_ram_addr = g_offline_dump_share_variable->dump_buffer_addr[index] + g_offline_dump_share_variable->dump_buffer_pos[index];
        offline_dump_region_write(OFFLINE_REGION_SYSLOG, curr_flash_addr,
                                  (uint8_t *)curr_ram_addr,
                                  g_offline_dump_share_variable->dump_buffer_size - g_offline_dump_share_variable->dump_buffer_pos[index]);
        curr_flash_addr += g_offline_dump_share_variable->dump_buffer_size - g_offline_dump_share_variable->dump_buffer_pos[index];
        offline_dump_region_write(OFFLINE_REGION_SYSLOG, curr_flash_addr,
                                  (uint8_t *)g_offline_dump_share_variable->dump_buffer_addr[index],
                                  g_offline_dump_share_variable->dump_buffer_pos[index]);
        offline_dump_region_write_end(OFFLINE_REGION_SYSLOG, g_offline_dump_share_variable->dump_buffer_size);
    } else {
        offline_dump_region_write(OFFLINE_REGION_SYSLOG, curr_flash_addr,
                                  (uint8_t *)g_offline_dump_share_variable->dump_buffer_addr[index],
                                  g_offline_dump_share_variable->dump_buffer_pos[index]);
        offline_dump_region_write_end(OFFLINE_REGION_SYSLOG, g_offline_dump_share_variable->dump_buffer_pos[index]);
    }
}

static void internal_log_save_to_flash(bool timeout_check, bool save_all, bool take_semphr)
{
    bool timeout_happen;
    static bool gpt_is_begin = false;
    static uint32_t begin_time;
    uint32_t index, curr_time, duration_time;

    if (save_all == false) {
        if (timeout_check == true) {
            /* Record the timeout information */
            if (gpt_is_begin == false) {
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &begin_time);
                gpt_is_begin = true;
            }
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_time);
            hal_gpt_get_duration_count(begin_time, curr_time, &duration_time);
            if (duration_time > SYSLOG_OFFLINE_DUMP_TIMEOUT) {
                timeout_happen = true;
            } else {
                timeout_happen = false;
            }
        }

        /*
         * case 1: there is NO log data in buffer at all, just return.
         * case 2: there are some log data in ongoing buffer, If timeout has happened and threshold is NOT reached, just return.
         * case 3: there are some log data in ongoing buffer, If timeout has happened and threshold reached, dump ongoing buffer to flash.
         * case 4: there are some log data in ready buffer, dump ready buffer to flash directly.
         */
        hal_nvic_save_and_set_interrupt_mask(&g_ram_flash_irq_mask);
        DUMP_SEMAPHORE_TAKE();
        if (g_offline_dump_share_variable->dump_buffer_ready == true) {
            /* There are some log data in ready buffer */
            index = internal_get_ready_dump_buffer();
        } else {
            index = g_offline_dump_share_variable->dump_ongoing_number;
            /* If NO data in ongoing buffer, just return */
            if ((g_offline_dump_share_variable->dump_buffer_pos[index] == 0) &&
                    (g_offline_dump_share_variable->dump_buffer_wrap[index] == false)) {
                DUMP_SEMAPHORE_GIVE();
                hal_nvic_restore_interrupt_mask(g_ram_flash_irq_mask);
                return;
            }
            /* If the threshold is NOT reached when timeout happen, just return. */
            if ((g_offline_dump_share_variable->dump_buffer_wrap[index] == false) &&
                    (g_offline_dump_share_variable->dump_buffer_pos[index] < ((g_offline_dump_share_variable->dump_buffer_size * 2) / 3))) {
                if ((timeout_check == true) && (timeout_happen == false)) {
                    DUMP_SEMAPHORE_GIVE();
                    hal_nvic_restore_interrupt_mask(g_ram_flash_irq_mask);
                    return;
                }
            }
            /* Swap the buffer to prapare the save operation */
            internal_dump_buffer_swap();
            g_offline_dump_share_variable->dump_buffer_ready = true;
        }
        DUMP_SEMAPHORE_GIVE();
        hal_nvic_restore_interrupt_mask(g_ram_flash_irq_mask);

        /* Copy the data from ready buffer to flash */
        internal_offline_dump_region_write(index);

        /* Reset the status of ready buffer */
        g_offline_dump_share_variable->dump_buffer_pos[index] = 0;
        g_offline_dump_share_variable->dump_buffer_wrap[index] = 0;
        g_offline_dump_share_variable->dump_buffer_ready = false;

        /* Re-calculate the timeout if this is caused by timeout condition */
        if ((timeout_check == true) && (timeout_happen == true)) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &begin_time);
        }
    } else {
        if (take_semphr == true) {
            hal_nvic_save_and_set_interrupt_mask(&g_ram_flash_irq_mask);
            DUMP_SEMAPHORE_TAKE();
        }
        /* Check both ready buffer and ongoing buffer if exception happen */
        if (g_offline_dump_share_variable->dump_buffer_ready == true) {
            index = internal_get_ready_dump_buffer();
            internal_offline_dump_region_write(index);
        }
        index = g_offline_dump_share_variable->dump_ongoing_number;
        if ((g_offline_dump_share_variable->dump_buffer_pos[index] != 0) ||
                (g_offline_dump_share_variable->dump_buffer_wrap[index] == true)) {
            internal_offline_dump_region_write(index);
        }
        if (take_semphr == true) {
            DUMP_SEMAPHORE_GIVE();
            hal_nvic_restore_interrupt_mask(g_ram_flash_irq_mask);
        }
    }
}

static void exception_syslog_offline_dump_callback(void)
{
    internal_log_save_to_flash(false, true, false);
}

void log_offline_dump_idle_hook(void)
{
    if (g_syslog_dump_is_active == true) {
        internal_log_save_to_flash(true, false, true);
    }
}

bool log_trigger_write_to_flash(void)
{
    if (g_syslog_dump_is_active == true) {
        return false;
    }

    internal_log_save_to_flash(false, true, true);

    return true;
}

void port_syslog_ram_flash_early_init(uint8_t port_index, hal_uart_baudrate_t baudrate)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(baudrate);

    /* help to record the baudrate if offline dump is disabled */
    g_sylog_uart_baudrate = baudrate;

    memset((uint8_t *)g_offline_dump_share_variable, 0, sizeof(offline_dump_share_variable_t));

    g_offline_dump_share_variable->dump_buffer_addr[0] = (uint32_t)g_syslog_dump_buffer[0];
    g_offline_dump_share_variable->dump_buffer_addr[1] = (uint32_t)g_syslog_dump_buffer[1];
    g_offline_dump_share_variable->dump_buffer_size = OFFLINE_REGION_SYSLOG_CELL_VALID_SIZE;
    g_offline_dump_share_variable->dump_buffer_pos[0] = 0;
    g_offline_dump_share_variable->dump_buffer_pos[1] = 0;
    g_offline_dump_share_variable->dump_buffer_wrap[0] = false;
    g_offline_dump_share_variable->dump_buffer_wrap[1] = false;
    g_offline_dump_share_variable->dump_buffer_ready = false;
    g_offline_dump_share_variable->dump_ongoing_number = 0;
}

void port_syslog_ram_flash_early_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);

    internal_copy_to_ram_buffer(buffer, size);
}

void port_syslog_ram_flash_exception_init(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

void port_syslog_ram_flash_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);
}

void port_syslog_ram_flash_post_init(uint8_t port_index, uint32_t share_buffer_addr, uint32_t share_buffer_size, pc_rx_handler_t rx_handler)
{
    exception_config_type exception_config;

    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(share_buffer_addr);
    PORT_SYSLOG_UNUSED(share_buffer_size);
    PORT_SYSLOG_UNUSED(rx_handler);

    offline_dump_region_init();

    exception_config.init_cb = exception_syslog_offline_dump_callback;
    exception_config.dump_cb = NULL;
    exception_register_callbacks(&exception_config);

    g_offline_dump_share_variable->share_buffer_addr = share_buffer_addr;
    g_offline_dump_share_variable->share_buffer_size = share_buffer_size;
    g_offline_dump_share_variable->share_buffer_pos = 0;
}

uint32_t port_syslog_ram_flash_receive_data(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);

    return 0;
}

#define SYSLOG_NVDM_DUMP_GROUP_NAME "syslog"
#define SYSLOG_NVDM_DUMP_DATA_NAME "dump"

/* This function is used to switch syslog between console and flash. */
bool log_path_switch(bool log_to_flash, bool save_to_nvdm, bool always_dump)
{
    log_dump_config_t log_dump_config;

    PORT_SYSLOG_UNUSED(save_to_nvdm);

    log_dump_config.log_to_flash = log_to_flash;
    log_dump_config.always_dump = always_dump;
    nvdm_write_data_item(SYSLOG_NVDM_DUMP_GROUP_NAME,
                         SYSLOG_NVDM_DUMP_DATA_NAME,
                         NVDM_DATA_ITEM_TYPE_RAW_DATA,
                         (const uint8_t *)&log_dump_config,
                         sizeof(log_dump_config));

    return true;
}

void port_syslog_device_get_setting(log_port_type_t *port_type, uint8_t *port_index)
{
    uint32_t max_size;
    log_dump_config_t log_dump_config;
    nvdm_status_t status;

    max_size = sizeof(log_dump_config);
    status = nvdm_read_data_item(SYSLOG_NVDM_DUMP_GROUP_NAME,
                                 SYSLOG_NVDM_DUMP_DATA_NAME,
                                 (uint8_t *)&log_dump_config,
                                 &max_size);
    if ((status == NVDM_STATUS_OK) && (log_dump_config.log_to_flash == false)) {
        return;
    }

    if (status == NVDM_STATUS_OK) {
        g_syslog_dump_is_active = log_dump_config.always_dump;
    } else {
        g_syslog_dump_is_active = false;
    }

    *port_type = LOG_PORT_TYPE_RAW_FLASH;
    *port_index = 0;
}

#endif

uint32_t port_syslog_ram_flash_get_hw_rptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);

    return g_offline_dump_share_variable->share_buffer_pos;
}

uint32_t port_syslog_ram_flash_get_hw_wptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);

    return g_offline_dump_share_variable->share_buffer_pos;
}

void port_syslog_ram_flash_set_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    uint8_t *log_start_addr;
    uint32_t index, curr_log_size;

    PORT_SYSLOG_UNUSED(port_index);

    DUMP_SEMAPHORE_TAKE();

    index = g_offline_dump_share_variable->dump_ongoing_number;
    if ((g_offline_dump_share_variable->dump_buffer_size - g_offline_dump_share_variable->dump_buffer_pos[index]) < move_bytes) {
        if (g_offline_dump_share_variable->dump_buffer_ready == false) {
            g_offline_dump_share_variable->dump_buffer_ready = true;
            internal_dump_buffer_swap();
        }
    }

    log_start_addr = (uint8_t *)memory_remap_primary_cpu_to_current_cpu(g_offline_dump_share_variable->share_buffer_addr);

    if ((g_offline_dump_share_variable->share_buffer_pos + move_bytes) >= g_offline_dump_share_variable->share_buffer_size) {
        curr_log_size = g_offline_dump_share_variable->share_buffer_size - g_offline_dump_share_variable->share_buffer_pos;
        internal_copy_to_ram_buffer(log_start_addr + g_offline_dump_share_variable->share_buffer_pos, curr_log_size);
        internal_copy_to_ram_buffer(log_start_addr, move_bytes - curr_log_size);
        g_offline_dump_share_variable->share_buffer_pos = move_bytes - curr_log_size;
    } else {
        internal_copy_to_ram_buffer(log_start_addr + g_offline_dump_share_variable->share_buffer_pos, move_bytes);
        g_offline_dump_share_variable->share_buffer_pos += move_bytes;
    }

    DUMP_SEMAPHORE_GIVE();
}

#else

#ifdef MTK_CPU_NUMBER_0
void port_syslog_ram_flash_early_init(uint8_t port_index, hal_uart_baudrate_t baudrate)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(baudrate);
}

void port_syslog_ram_flash_early_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);
}

void port_syslog_ram_flash_exception_init(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

void port_syslog_ram_flash_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);
}

void port_syslog_ram_flash_post_init(uint8_t port_index, uint32_t share_buffer_addr, uint32_t share_buffer_size, pc_rx_handler_t rx_handler)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(share_buffer_addr);
    PORT_SYSLOG_UNUSED(share_buffer_size);
    PORT_SYSLOG_UNUSED(rx_handler);
}

uint32_t port_syslog_ram_flash_receive_data(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);

    return 0;
}

#endif

uint32_t port_syslog_ram_flash_get_hw_rptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);

    return 0;
}

uint32_t port_syslog_ram_flash_get_hw_wptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);

    return 0;
}

void port_syslog_ram_flash_set_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(move_bytes);
}

#endif

void port_syslog_ram_flash_init_hook(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

void port_syslog_ram_flash_logging_hook(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

port_syslog_device_ops_t g_port_syslog_ram_flash_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_syslog_ram_flash_early_init,
    port_syslog_ram_flash_early_send,
    port_syslog_ram_flash_post_init,
    port_syslog_ram_flash_exception_init,
    port_syslog_ram_flash_exception_send,
    port_syslog_ram_flash_receive_data,
#endif
    port_syslog_ram_flash_init_hook,
    port_syslog_ram_flash_logging_hook,
    port_syslog_ram_flash_get_hw_rptr,
    port_syslog_ram_flash_get_hw_wptr,
    port_syslog_ram_flash_set_hw_wptr,
};

