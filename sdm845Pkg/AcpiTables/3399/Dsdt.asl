/** @file

  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2018, Linaro Ltd. All rights reserved.<BR>
  Copyright (c) 2019, Andrey Warkentin <andrey.warkentin@gmail.com>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock ("DSDT.aml", "DSDT", 2, "RKCP  ", "RK3399  ", 3)
{
    Scope (_SB)
    {
        Device (CPU0)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x000)  // _UID: Unique ID
        }
        Device (CPU1)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x001)  // _UID: Unique ID
        }
        Device (CPU2)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x002)  // _UID: Unique ID
        }
        Device (CPU3)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x003)  // _UID: Unique ID
        }
        Device (CPU4)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x101)  // _UID: Unique ID
        }
        Device (CPU5)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x102)  // _UID: Unique ID
        }

        Device (XHC0)
        {
            Name (_HID, "PNP0D15")      // _HID: Hardware ID
            Name (_UID, 0x00)           // _UID: Unique ID
            Name (_CCA, 0x00)           // Not coherent!

            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xfe800000,         // Address Base (MMIO)
                    0x00008000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                {
                  137
                }
            })
        }

        Device (XHC1)
        {
            Name (_HID, "PNP0D15")      // _HID: Hardware ID
            Name (_UID, 0x01)           // _UID: Unique ID
            Name (_CCA, 0x00)           // Not coherent!

            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xfe900000,         // Address Base (MMIO)
                    0x00008000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                {
                  142
                }
            })
        }

        Device (COM1)
        {
            Name (_HID, "RKCP8250")                             // _HID: Hardware ID
            Name (_CID, Package() {"HISI0031", "8250dw"})       // _CID: Compatible ID
            Name (_ADR, FixedPcdGet64(PcdSerialRegisterBase))   // _ADR: Address
            Name (_CRS, ResourceTemplate ()                     // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    FixedPcdGet64(PcdSerialRegisterBase),       // Address Base
                    0x00000100,                                 // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                {
                  132
                }
            })
        }
    }
}
