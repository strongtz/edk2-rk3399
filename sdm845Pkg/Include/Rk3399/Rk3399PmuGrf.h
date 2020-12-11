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

#ifndef __RK3399_PMU_GRF_H__
#define __RK3399_PMU_GRF_H__

/* PMU grf */
#define PMU_GRF_GPIO0A_IOMUX	0x0000
#define PMU_GRF_GPIO0B_IOMUX	0x0004

#define PMU_GRF_GPIO1A_IOMUX	0x0010
#define PMU_GRF_GPIO1B_IOMUX	0x0014
#define PMU_GRF_GPIO1C_IOMUX	0x0018
#define PMU_GRF_GPIO1D_IOMUX	0x001C

#define PMU_GRF_GPIO0A_P	0x0040
#define PMU_GRF_GPIO0B_P	0x0044

#define PMU_GRF_GPIO1A_P	0x0050
#define PMU_GRF_GPIO1B_P	0x0054
#define PMU_GRF_GPIO1C_P	0x0058
#define PMU_GRF_GPIO1D_P	0x005C

#define PMU_GRF_SOC_CON0	0x0180
#define PMU_GRF_SOC_CON10	0x01A8
#define PMU_GRF_SOC_CON11	0x01AC

#define PMU_GRF_OS_REG0		0x0300
#define PMU_GRF_OS_REG1		0x0304
#define PMU_GRF_OS_REG2		0x0308
#define PMU_GRF_OS_REG3		0x030C

#endif  //__RK3399_PMU_GRF_H__
