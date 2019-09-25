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

#if defined(MTK_MINICLI_ENABLE)
#ifdef MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE

#include "hal_trng.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include <stdlib.h>
#include "hal_gpt.h"
#include "toi.h"
#include "exception_handler.h"
#include "offline_dump_cli.h"

static uint32_t g_assert_during_time, g_log_during_time;

uint32_t random_get_value(uint32_t min, uint32_t max)
{
    uint32_t random_count;

    while (hal_trng_init() != HAL_TRNG_STATUS_OK) {
        vTaskDelay(50);
    }

    do {
        hal_trng_get_generated_random_number(&random_count);
        random_count %= max;
    } while (random_count < min);

    hal_trng_deinit();

    return random_count;
}

static void offline_dump_test_task1(void *pvParameters)
{
    while (1) {
        LOG_I(common, "hello world at tick %d", xTaskGetTickCount());
        vTaskDelay(random_get_value(50, 100));
    }
}

static void offline_dump_test_task2(void *pvParameters)
{
    uint32_t syslog_count = 1, gpt_start_count, gpt_curr_count;

    srand(100);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);

    while (1) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_curr_count);
        if (((gpt_curr_count + random_get_value(5 * 1000000, 10 * 1000000)) - gpt_start_count) > (g_assert_during_time * 1000000)) {
            assert(0);
        }
        if (!(syslog_count++ % 10)) {
            log_trigger_write_to_flash();
        }
        vTaskDelay(random_get_value(g_log_during_time, g_log_during_time + 100));
    }
}

static uint8_t offline_dump_test(uint8_t len, char *param[])
{
    uint8_t type;

    if (len != 2) {
        cli_puts("parameter error");
        cli_putln();
        return 0;
    }

    exception_dump_config(DISABLE_WHILELOOP_MAGIC);

    g_log_during_time = toi(param[0], &type);
    g_assert_during_time = toi(param[1], &type);

    xTaskCreate(offline_dump_test_task1, "dump_test1", 2048 / sizeof(StackType_t), NULL, TASK_PRIORITY_HIGH, NULL);
    xTaskCreate(offline_dump_test_task2, "dump_test2", 2048 / sizeof(StackType_t), NULL, TASK_PRIORITY_HIGH, NULL);

    cli_puts("offline_dump_test_task2 creat done");
    cli_putln();

    return 0;
}

cmd_t offline_dump_cli[] = {
    {"active", "offline_dump active <log save time(ms)> <assert time(s)>", offline_dump_test, NULL},
    {NULL,     NULL,            NULL,                    NULL}
};

#endif
#endif

