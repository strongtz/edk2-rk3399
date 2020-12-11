/** @file
*
*  Copyright (c) 2011 - 2014, ARM Limited. All rights reserved.
*  Copyright (c) 2017, Rockchip Inc. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/


#ifndef __RK808_H__
#define __RK808_H__

#define RK808_SECONDS_REG 0x00
#define RK808_MINUTES_REG 0x01
#define RK808_HOURS_REG 0x02
#define RK808_DAYS_REG 0x03
#define RK808_MONTHS_REG 0x04
#define RK808_YEARS_REG 0x05
#define RK808_WEEKS_REG 0x06
#define RK808_ALARM_SECONDS_REG 0x07
#define RK808_ALARM_MINUTES_REG 0x08
#define RK808_ALARM_HOURS_REG 0x09
#define RK808_ALARM_DAYS_REG 0x0a
#define RK808_ALARM_MONTHS_REG 0x0b
#define RK808_ALARM_YEARS_REG 0x0c
#define RK808_RTC_CTRL_REG 0x10
#define RK808_RTC_STATUS_REG 0x11
#define RK808_RTC_INT_REG 0x12
#define RK808_RTC_COMP_LSB_REG 0x13
#define RK808_RTC_COMP_MSB_REG 0x14

#define RK808_DCDC_EN_REG      0x23
#define RK808_LDO_EN_REG       0x24

#define RK808_BUCK1_ON_VSEL_REG 0x2F
#define RK808_BUCK2_ON_VSEL_REG 0x33
#define RK808_BUCK4_ON_VSEL_REG 0x38
#define RK808_LDO1_ON_VSEL_REG 0x3B
#define RK808_LDO2_ON_VSEL_REG 0x3D
#define RK808_LDO3_ON_VSEL_REG 0x3F
#define RK808_LDO4_ON_VSEL_REG 0x41
#define RK808_LDO5_ON_VSEL_REG 0x43
#define RK808_LDO6_ON_VSEL_REG 0x45
#define RK808_LDO7_ON_VSEL_REG 0x47
#define RK808_LDO8_ON_VSEL_REG 0x49


#endif  //__RK808_H__
