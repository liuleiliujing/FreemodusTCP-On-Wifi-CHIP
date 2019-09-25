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
 
#include "fota_util.h"
#include "fota_multi_info.h"
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
#include "bsp_external_flash.h"
#include <assert.h>
#endif
#include "fota_flash.h"
#include "fota_multi_info_util.h"

fota_flash_partition_info *g_flash_partition_info = NULL;
FotaStorageType g_flash_storage_type = InternalFlash;
uint8_t g_flash_is_inuse = 0;


void fota_flash_set_storage_type(FotaStorageType storage_type)
{
    if (InternalFlash == storage_type ||
        ExternalFlash == storage_type)
    {
        g_flash_storage_type = storage_type;
    }
}


FotaStorageType fota_flash_get_storage_type(void)
{
    return g_flash_storage_type;
}


void fota_flash_bootup(void)
{
    /* Init internal Flash */
    fota_flash_init(TRUE);
    /* Init external Flash */
    fota_flash_init(FALSE);
    
    /* Set flash partition information. */
    fota_flash_config_init(&g_flash_partition_info);

#if FOTA_STORE_IN_EXTERNAL_FLASH
    fota_flash_set_storage_type(ExternalFlash);
#else
    fota_flash_set_storage_type(InternalFlash);
#endif

    fota_multi_info_sector_init();
}


#ifdef BL_FOTA_ENABLE
bool fota_flash_is_addr_range_valid(uint32_t addr, uint32_t length)
{
    uint32_t i = 0;

    while (g_flash_partition_info && 
           FLASH_PARTITION_TYPE_MAX != g_flash_partition_info[i].partition_type)
    {
        if (addr >= g_flash_partition_info[i].LoadAddressLow && 
            (addr + length <= g_flash_partition_info[i].LoadAddressLow + g_flash_partition_info[i].BinaryLengthLow))
        {
            return TRUE;
        }
        i++;
    }

    FOTA_LOG_W("Invalid address range. i:%d addr:%x len:%d", i, addr, length);
    return FALSE;
}
#endif


fota_flash_partition_info *fota_flash_get_partition_info(flash_partition_type_enum partition_type)
{
    uint32_t i = 0;

    while (g_flash_partition_info && 
           FLASH_PARTITION_TYPE_MAX != g_flash_partition_info[i].partition_type &&
           partition_type != g_flash_partition_info[i].partition_type)
    {
        i++;
    }

    if (g_flash_partition_info && partition_type == g_flash_partition_info[i].partition_type)
    {
#ifndef MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
        FOTA_LOG_I("Found, partition_type:%d", partition_type);
#endif
        return g_flash_partition_info + i;
    }

#ifndef MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
    FOTA_LOG_W("Not Found, partition_type:%d", partition_type);
#endif

    return NULL;
}


FOTA_ERRCODE fota_flash_get_fota_partition_info(FotaStorageType * storage_type, uint32_t * fota_address, uint32_t * length)
{
    if (!storage_type || !fota_address || !length)
    {
        return FOTA_ERRCODE_FAIL;
    }

    *storage_type = fota_flash_get_storage_type();

    if (ExternalFlash == *storage_type)
    {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        // TODO: Could be optimized.
        fota_flash_partition_info *partition_info = fota_flash_get_partition_info(FLASH_PARTITION_TYPE_FOTA_EXT);
        if (!partition_info)
        {
            return FOTA_ERRCODE_FAIL;
        }

        *fota_address = partition_info->LoadAddressLow;
        *length = partition_info->BinaryLengthLow;
#else
        return FOTA_ERRCODE_UNSUPPORTED;
#endif
    }
    else if (InternalFlash == *storage_type)
    {
        fota_flash_partition_info *partition_info = fota_flash_get_partition_info(FLASH_PARTITION_TYPE_FOTA);
        if (!partition_info)
        {
            return FOTA_ERRCODE_FAIL;
        }

        *fota_address = partition_info->LoadAddressLow;
        *length = partition_info->BinaryLengthLow;
    }
    else
    {
        return FOTA_ERRCODE_UNSUPPORTED;
    }

    return FOTA_ERRCODE_SUCCESS;
}


FOTA_ERRCODE fota_flash_init(bool is_int)
{
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;
    static bool is_hal_inited = FALSE;

    if (is_int)
    {
        if (!is_hal_inited)
        {
            ret = HAL_FLASH_STATUS_OK == hal_flash_init() ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
            is_hal_inited = TRUE;
        }
        else
        {   
            ret = FOTA_ERRCODE_SUCCESS;
        }
    }
    else
    {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        /* Do not care about the return value for other task may have initialzed the SPI external Flash and SPI external Flash does
                * not support call init API more than one time without calling enough number of deinit API in the midway.
                * Only support 'init-deinit-init' flow. BSP_FLASH_STATUS_ERROR will be returned if init without deinit.
                */
        ret = bsp_external_flash_init(HAL_SPI_MASTER_2, 50 * 1000000);
        FOTA_LOG_I("bsp_external_flash_init() ret:%d", ret);
        ret = FOTA_ERRCODE_SUCCESS;
#else
        ret = FOTA_ERRCODE_UNSUPPORTED;
#endif
    }

    return ret;
}


#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
static block_size_type_t fota_flash_block_type_cnvt(hal_flash_block_t block_type)
{
    switch (block_type)
    {
        case HAL_FLASH_BLOCK_4K:
            return FLASH_BLOCK_4K;

        case HAL_FLASH_BLOCK_32K:
            return FLASH_BLOCK_32K;

        case HAL_FLASH_BLOCK_64K:
            return FLASH_BLOCK_64K;

        default:
            assert(0);
            break;
    }

    return FLASH_BLOCK_4K;
}
#endif


FOTA_ERRCODE fota_flash_erase(uint32_t start_address, hal_flash_block_t block_type, bool is_int)
{
    int32_t ret = FOTA_ERRCODE_FAIL;
    
    if (is_int)
    {
        ret = hal_flash_erase(start_address, block_type);
        return HAL_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
    }
    else
    {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        ret = bsp_external_flash_erase(start_address,
                                       fota_flash_block_type_cnvt(block_type));
        if (BSP_FLASH_STATUS_NOT_INIT == ret)
        {
            /* Other task may deinit external Flash. */
            bsp_external_flash_init(HAL_SPI_MASTER_2, 50 * 1000000);
            ret = bsp_external_flash_erase(start_address, 
                                           fota_flash_block_type_cnvt(block_type));
        }
        return BSP_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
#else
        return FOTA_ERRCODE_UNSUPPORTED;
#endif
    }
}


// TODO: fota_port_isr_disable() and fota_set_scramble_flag()
FOTA_ERRCODE fota_flash_write(uint32_t address, const uint8_t *data, uint32_t length, bool is_int)
{
    int32_t ret = FOTA_ERRCODE_FAIL;

    if (is_int)
    {
        ret = hal_flash_write(address, data, length);
        return HAL_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
    }
    else
    {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        ret = bsp_external_flash_write(address, data, length);
        if (BSP_FLASH_STATUS_NOT_INIT == ret)
        {
            /* Other task may deinit external Flash. */
            bsp_external_flash_init(HAL_SPI_MASTER_2, 50 * 1000000);
            ret = bsp_external_flash_write(address, data, length);
        }
        return BSP_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
#else
        return FOTA_ERRCODE_UNSUPPORTED;
#endif
    }
}


FOTA_ERRCODE fota_flash_read(uint32_t start_address, uint8_t *buffer, uint32_t length, bool is_int)
{
    int32_t ret = FOTA_ERRCODE_FAIL;
    
    if (is_int)
    {
        ret = hal_flash_read(start_address, buffer, length);
        return HAL_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
    }
    else
    {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        ret = bsp_external_flash_read(start_address, buffer, length);
        if (BSP_FLASH_STATUS_NOT_INIT == ret)
        {
            /* Other task may deinit external Flash. */
            bsp_external_flash_init(HAL_SPI_MASTER_2, 50 * 1000000);
            ret = bsp_external_flash_read(start_address, buffer, length);
        }
        return BSP_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
#else
        return FOTA_ERRCODE_UNSUPPORTED;
#endif
    }
}


void fota_flash_set_flag_inuse(uint8_t is_use)
{
	FOTA_LOG_I("fota_flash_set_use(): is_use:%d, cur_flag = %d", is_use, g_flash_is_inuse);
	g_flash_is_inuse = is_use;
}


uint8_t fota_flash_query_is_inuse()
{
	return g_flash_is_inuse;
}


static FOTA_ERRCODE fota_flash_is_4K_block_erased(bool *is_erased, uint32_t start_address, bool is_int)
{
    uint8_t buffer[128] = {0};
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;
    uint8_t i = 0, j = 0;

    if (start_address & 0xFFF || !is_erased)
    {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    *is_erased = FALSE;

    /* 4*1024/128 = 32 */
    for (i = 0; i < 32; i++)
    {
        ret = fota_flash_read(start_address, buffer, 128, is_int);
        if (FOTA_ERRCODE_SUCCESS != ret)
        {
            return ret;
        }

        for (j = 0; j < 128; j++)
        {
            if (0xFF != buffer[j])
            {
                return FOTA_ERRCODE_SUCCESS;
            }
        }
    }

    *is_erased = TRUE;
    return FOTA_ERRCODE_SUCCESS;
}


/* If erase_status is NULL and erase_status_size is not, return the size only. 
  * If erase_status is not NULL, originally erase_status_size is the size of erase_status buffer.
  * And it will be updated to be the actual size needed.
  */
FOTA_ERRCODE fota_flash_get_partition_erase_status(uint8_t *erase_status,
                                                           uint16_t *erase_status_size,
                                                           uint32_t start_address,
                                                           uint32_t length,
                                                           bool is_int)
{
    uint32_t bit_size = 0, curr_bit_pos = 0;
    bool is_erased = FALSE;
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;
    uint16_t erase_status_size_old = 0;

    FOTA_LOG_I("erase_status:%x,erase_status_size:%x,start_address:%x,length:%x,is_int:%d",
               erase_status,
               erase_status_size,
               start_address,
               length,
               is_int);

    if (!erase_status_size ||
#ifndef FOTA_ERASE_VERIFY_LEN
        start_address & 0xFFF)
#else
        start_address & 0xFFF || length & 0xFFF)
#endif
    {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    erase_status_size_old = *erase_status_size;

#ifndef FOTA_ERASE_VERIFY_LEN
    bit_size = ((length >> 12) + ((length & 0xFFF) > 0 ? 1 : 0));
#else
    bit_size = length >> 12;
#endif
    *erase_status_size = ((bit_size >> 3) + (bit_size % 8 ? 1 : 0));

    FOTA_LOG_I("bit_size:%d, *erase_status_size:%d", bit_size, *erase_status_size);
    if (!erase_status)
    {
        /* Return erase_status_size only. */
        return FOTA_ERRCODE_SUCCESS;
    }

    FOTA_LOG_I("erase_status_size_old:%d, *erase_status_size:%d",
               erase_status_size_old,
               *erase_status_size);
    if (erase_status_size_old < *erase_status_size)
    {
        return FOTA_ERRCODE_FAIL;
    }

    memset(erase_status, 0, erase_status_size_old);

    curr_bit_pos = 0;

    while (curr_bit_pos < bit_size)
    {
        ret = fota_flash_is_4K_block_erased(&is_erased, start_address, is_int);
        if (FOTA_ERRCODE_SUCCESS != ret)
        {
            return ret;
        }

        if (is_erased)
        {
            erase_status[curr_bit_pos / 8] |= (0x80 >> (curr_bit_pos % 8));
        }

        curr_bit_pos++;
        start_address += 0x1000;
    }

    for (int k = 0; k < *erase_status_size; k++)
    {
        FOTA_LOG_I("erase_status[%d]:%x", k, erase_status[k]);
    }

    return FOTA_ERRCODE_SUCCESS;
}

