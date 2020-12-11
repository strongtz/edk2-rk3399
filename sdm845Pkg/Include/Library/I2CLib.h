/** @file
*
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

#ifndef _I2C_LIB_H_
#define _I2C_LIB_H_

/* i2c bus chanel */
enum RkI2CBusID {
	I2C_CH0,  //I2C0_PMU
	I2C_CH1,
	I2C_CH2,
	I2C_CH3,
	I2C_CH4,
	I2C_CH5,
	I2C_CH6,
	I2C_CH7,

	I2C_BUS_MAX
};

struct RkI2CInfo {
	UINT32		Regs;
	UINT32		Speed;
};

typedef struct {
    UINT32           BusID;
    UINT32           Speed;
    UINT32           SlaveDeviceAddress;
}I2C_DEVICE;

EFI_STATUS
RkI2cLibRuntimeSetup(enum RkI2CBusID BusId);

void *RkI2CGetBase(enum RkI2CBusID BusId);

EFI_STATUS
EFIAPI
I2CInit(  UINT32 BusId, UINT32 Speed);

EFI_STATUS
EFIAPI
I2CWrite(UINT32 BusId, UINT8 Chip, UINT32 Addr, UINT32 Alen, UINT8 *Buf, UINT32 Len);

EFI_STATUS
EFIAPI
I2CRead(UINT32 BusId, UINT8 Chip, UINT32 Addr, UINT32 Alen, UINT8 *Buf, UINT32 Len);

#endif
