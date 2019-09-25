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
 
#ifndef __FOTA_PLATFORM_H__
#define __FOTA_PLATFORM_H__
#include <stdint.h>
#include "hal_flash.h"

#define UNUSED(n)

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

#define FLASH_ADDR uint32_t
#define BOOL bool
#define VOID void
#define NVRam_DByteRead_ERROR FOTA_ERRCODE_FAIL


#define EXTERN extern

#ifndef ALIGN
#define ALIGN(n)  __attribute__((aligned((n))))
#endif

#define NVRam_DByteRead(start_address, buffer, length)    fota_flash_read((start_address), (buffer), (length), TRUE)
#define NVRam_D64KErase(start_address)                    fota_flash_erase((start_address), HAL_FLASH_BLOCK_64K, TRUE)
#define NVRam_D32KErase(start_address)                    fota_flash_erase((start_address), HAL_FLASH_BLOCK_32K, TRUE)
#define NVRam_D4KErase(start_address)                     fota_flash_erase((start_address), HAL_FLASH_BLOCK_4K, TRUE)
#define NVRam_DPageRead(start_address,buffer)             fota_flash_read((start_address),(buffer), 256, TRUE)
#define NVRam_DPageWrite(start_address,buffer)            fota_flash_write((start_address),(buffer), 256, TRUE)

#define FLASH_ERRCODE      int32
#define FLASH_SUCCESS      (0)
#define FLASH_PAGE_SIZE    (256)

#endif /* __FOTA_PLATFORM_H__ */

