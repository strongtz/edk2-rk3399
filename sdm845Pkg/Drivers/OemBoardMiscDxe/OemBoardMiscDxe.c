/** @file

  Copyright (c) 2014-2017, Linaro Limited. All rights reserved.
  Copyright (c) 2017, Rockchip Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Rk3399/Rk3399.h>

EFI_STATUS
OemBoardMiscDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  /* temporary workaround for PWM */
  MmioWrite32(RK3399_PWM_BASE + 0x24, 0x000004b7);
  MmioWrite32(RK3399_PWM_BASE + 0x28, 0x0000027f);
  MmioWrite32(RK3399_PWM_BASE + 0x2c, 0x00000013);

  MmioWrite32(RK3399_PWM_BASE + 0x34, 0x000004b7);
  MmioWrite32(RK3399_PWM_BASE + 0x38, 0x0000027f);
  MmioWrite32(RK3399_PWM_BASE + 0x3c, 0x00000013);

  return EFI_SUCCESS;
}
