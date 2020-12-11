/** @file
*
*  Copyright (c) 2014-2017, Linaro Limited. All rights reserved.
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

#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

#include <Rk3399/Rk3399.h>
#include <Rk3399/Rk3399PmuGrf.h>
#include "Rk3399Mem.h"

// The total number of descriptors, including the final "end-of-table" descriptor.
#define MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS 12

// DDR attributes
#define DDR_ATTRIBUTES_CACHED           ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED         ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

#define EXTRA_SYSTEM_MEMORY_BASE  0x40000000

STATIC struct Rk3399ReservedMemory {
  EFI_PHYSICAL_ADDRESS         Offset;
  EFI_PHYSICAL_ADDRESS         Size;
} Rk3399ReservedMemoryBuffer [] = {
  { 0x00000000, 0x200000 }    // Reserved for ATF
};

STATIC
UINT64
EFIAPI
Rk3399InitMemorySize (
  IN VOID
  )
{
  UINT32 Rank, Col, Bank, Cs0Row, Cs1Row, Bw, Row34;
  UINT32 ChipSizeMb = 0;
  UINT64 SizeMb = 0;
  UINT32 Ch;
  UINT64 ret;

  UINT32 SysReg = MmioRead32(RK3399_PMU_GRF_BASE + PMU_GRF_OS_REG2);
  UINT32 ChNum = 1 + ((SysReg >> SYS_REG_NUM_CH_SHIFT) &
                      SYS_REG_NUM_CH_MASK);

  for (Ch = 0; Ch < ChNum; Ch++) {
    Rank = 1 + (SysReg >> SYS_REG_RANK_SHIFT(Ch) &
                SYS_REG_RANK_MASK);
    Col = 9 + (SysReg >> SYS_REG_COL_SHIFT(Ch) & SYS_REG_COL_MASK);
    Bank = 3 - ((SysReg >> SYS_REG_BK_SHIFT(Ch)) & SYS_REG_BK_MASK);
    Cs0Row = 13 + (SysReg >> SYS_REG_CS0_ROW_SHIFT(Ch) &
                   SYS_REG_CS0_ROW_MASK);
    Cs1Row = 13 + (SysReg >> SYS_REG_CS1_ROW_SHIFT(Ch) &
                   SYS_REG_CS1_ROW_MASK);
    Bw = (2 >> ((SysReg >> SYS_REG_BW_SHIFT(Ch)) &
                SYS_REG_BW_MASK));
    Row34 = SysReg >> SYS_REG_ROW_3_4_SHIFT(Ch) &
            SYS_REG_ROW_3_4_MASK;

    ChipSizeMb = (1 << (Cs0Row + Col + Bank + Bw - 20));

    if (Rank > 1)
      ChipSizeMb += ChipSizeMb >> (Cs0Row - Cs1Row);
    if (Row34)
      ChipSizeMb = ChipSizeMb * 3 / 4;
    SizeMb += ChipSizeMb;

    DEBUG((DEBUG_INFO, "Rank %d Col %d Bank %d Cs0Row %d Bw %d Row34 %d\n",
          Rank, Col, Bank, Cs0Row, Bw, Row34));
  }

  /*
   * Support maximum DDR capacity is 4GB size, less the MMIO hole
   * at 0xf8000000, where the SoC registers are.
   */
  ret = SizeMb << 20;

  if (ret >= RK3399_PERIPH_BASE) {
    ret = RK3399_PERIPH_BASE;
  }

  DEBUG((DEBUG_INFO, "memory size=%dMB 0x%x\n", SizeMb, ret));

  return ret;
}

/**
  Return the Virtual Memory Map of your platform

  This Virtual Memory Map is used by MemoryInitPei Module to initialize the MMU on your platform.

  @param[out]   VirtualMemoryMap    Array of ARM_MEMORY_REGION_DESCRIPTOR describing a Physical-to-
                                    Virtual Memory mapping. This array must be ended by a zero-filled
                                    entry

**/
VOID
ArmPlatformGetVirtualMemoryMap (
  IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
  )
{
  ARM_MEMORY_REGION_ATTRIBUTES  CacheAttributes;
  UINTN                         Index = 0, Count, ReservedTop;
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;
  EFI_PEI_HOB_POINTERS          NextHob;
  EFI_RESOURCE_ATTRIBUTE_TYPE   ResourceAttributes;
  UINT64                        ResourceLength;
  EFI_PHYSICAL_ADDRESS          ResourceTop;
  UINT64                        MemorySize, AdditionalMemorySize;

  MemorySize = Rk3399InitMemorySize ();
  if (MemorySize == 0) {
    MemorySize = PcdGet64 (PcdSystemMemorySize);
  }

  ResourceAttributes = (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_TESTED
  );

  // Create initial Base Hob for system memory.
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    ResourceAttributes,
    PcdGet64 (PcdSystemMemoryBase),
    PcdGet64 (PcdSystemMemorySize)
  );

  NextHob.Raw = GetHobList ();
  Count = sizeof (Rk3399ReservedMemoryBuffer) / sizeof (struct Rk3399ReservedMemory);
  while ((NextHob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, NextHob.Raw)) != NULL) {
    if (Index >= Count) {
      break;
    }
    if ((NextHob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) &&
        (Rk3399ReservedMemoryBuffer[Index].Offset >= NextHob.ResourceDescriptor->PhysicalStart) &&
        ((Rk3399ReservedMemoryBuffer[Index].Offset + Rk3399ReservedMemoryBuffer[Index].Size) <=
         NextHob.ResourceDescriptor->PhysicalStart + NextHob.ResourceDescriptor->ResourceLength)) {
      ResourceAttributes = NextHob.ResourceDescriptor->ResourceAttribute;
      ResourceLength = NextHob.ResourceDescriptor->ResourceLength;
      ResourceTop = NextHob.ResourceDescriptor->PhysicalStart + ResourceLength;
      ReservedTop = Rk3399ReservedMemoryBuffer[Index].Offset + Rk3399ReservedMemoryBuffer[Index].Size;

      // Create the System Memory HOB for the reserved buffer
      BuildResourceDescriptorHob (EFI_RESOURCE_MEMORY_RESERVED,
                                  EFI_RESOURCE_ATTRIBUTE_PRESENT,
                                  Rk3399ReservedMemoryBuffer[Index].Offset,
                                  Rk3399ReservedMemoryBuffer[Index].Size);
      // Update the HOB
      NextHob.ResourceDescriptor->ResourceLength = Rk3399ReservedMemoryBuffer[Index].Offset - NextHob.ResourceDescriptor->PhysicalStart;

      // If there is some memory available on the top of the reserved memory then create a HOB
      if (ReservedTop < ResourceTop) {
        BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                                    ResourceAttributes,
                                    ReservedTop,
                                    ResourceTop - ReservedTop);
      }
      Index++;
    }
    NextHob.Raw = GET_NEXT_HOB (NextHob);
  }

  AdditionalMemorySize = MemorySize - PcdGet64 (PcdSystemMemorySize);
  if (AdditionalMemorySize > 0) {
    // Declared the additional memory
    ResourceAttributes =
      EFI_RESOURCE_ATTRIBUTE_PRESENT |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_TESTED;

    BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      ResourceAttributes,
      EXTRA_SYSTEM_MEMORY_BASE,
      AdditionalMemorySize);
  }

  ASSERT (VirtualMemoryMap != NULL);

  VirtualMemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR*)AllocatePages(EFI_SIZE_TO_PAGES (sizeof(ARM_MEMORY_REGION_DESCRIPTOR) * MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS));
  if (VirtualMemoryTable == NULL) {
    return;
  }

  CacheAttributes = DDR_ATTRIBUTES_CACHED;

  Index = 0;

  // RK3399 SOC peripherals
  VirtualMemoryTable[Index].PhysicalBase    = RK3399_PERIPH_BASE;
  VirtualMemoryTable[Index].VirtualBase     = RK3399_PERIPH_BASE;
  VirtualMemoryTable[Index].Length          = RK3399_PERIPH_SZ;
  VirtualMemoryTable[Index].Attributes      = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

  // DDR - predefined 1GB size
  VirtualMemoryTable[++Index].PhysicalBase  = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].VirtualBase     = PcdGet64 (PcdSystemMemoryBase);
  VirtualMemoryTable[Index].Length          = PcdGet64 (PcdSystemMemorySize);
  VirtualMemoryTable[Index].Attributes      = CacheAttributes;

  // If DDR capacity is over 1GB.
  if (AdditionalMemorySize > 0) {
    VirtualMemoryTable[++Index].PhysicalBase = EXTRA_SYSTEM_MEMORY_BASE;
    VirtualMemoryTable[Index].VirtualBase    = EXTRA_SYSTEM_MEMORY_BASE;
    VirtualMemoryTable[Index].Length         = AdditionalMemorySize;
    VirtualMemoryTable[Index].Attributes     = CacheAttributes;
  }

  // End of Table
  VirtualMemoryTable[++Index].PhysicalBase  = 0;
  VirtualMemoryTable[Index].VirtualBase     = 0;
  VirtualMemoryTable[Index].Length          = 0;
  VirtualMemoryTable[Index].Attributes      = (ARM_MEMORY_REGION_ATTRIBUTES)0;

  ASSERT((Index + 1) <= MAX_VIRTUAL_MEMORY_MAP_DESCRIPTORS);

  *VirtualMemoryMap = VirtualMemoryTable;
}
