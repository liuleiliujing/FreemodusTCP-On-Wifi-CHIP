/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_AUDIO_TEST_H__
#define __HAL_AUDIO_TEST_H__

#include "hal_gpt.h"
#include "hal_log.h"
#include "hal_pmu.h"
#include "hal_audio.h"
#include "Audio_FFT.h"
#define FFT_BUFFER_SIZE 256
#define ResLen(cpyidx,buf)             (FFT_BUFFER_SIZE - ((uint16_t*)cpyidx - (uint16_t*)&buf[0]))
#define ch_idx                         (smt_curr & SMT_CH_LEFT ? 0:1)

static const uint16_t empty_feature[2] = {0x0,0x0};
typedef enum smt_chennel_e{
    SMT_CH_LEFT = 0x1,
    SMT_CH_RIGHT = 0x2,
    SMT_CH_NONE = 0x4,
}smt_ch;

typedef enum mic_driver_e{
    DMIC_DRV = 0x0,
    AMIC_DRV
}mic_drv;

typedef union {
    uint8_t value;
    struct {
        uint8_t curr_ch:4;
        uint8_t curr_mic:4;
    }filed;
}smt_status;

typedef struct {
    kal_uint32 u4Freq_data;
    kal_uint32 u4Mag_data;
    uint8_t aud_preampl_gain[2];
    uint16_t bitstream_buf[FFT_BUFFER_SIZE];
    uint16_t *cpyIdx;
    uint8_t chennel;
}fft_buf_t ;


void audio_smt_test(bool enable,smt_ch);
void audio_smt_test_pure_on_off(bool ,smt_ch );
void hal_audio_switch_mic_driver(mic_drv drv);
void hal_audio_init_stream_buf(fft_buf_t *);


#endif /*defined(__GNUC__)*/
