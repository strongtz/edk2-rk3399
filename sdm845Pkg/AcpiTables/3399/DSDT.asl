/*
 * Copyright (c) 2020, Xilin Wu <strongtz@yeah.net>
 * 
 */
DefinitionBlock ("", "DSDT", 5, "RKCP  ", "RKCPEDK2", 0x00003399)
{
    Scope (_SB)
    {
        Device (CPU0)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, Zero)  // _UID: Unique ID
        }
#if 0
        Device (CPU1)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, One)  // _UID: Unique ID
        }

        Device (CPU2)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x02)  // _UID: Unique ID
        }

        Device (CPU3)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x03)  // _UID: Unique ID
        }

        Device (CPU4)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x04)  // _UID: Unique ID
        }

        Device (CPU5)
        {
            Name (_HID, "ACPI0007" /* Processor Device */)  // _HID: Hardware ID
            Name (_UID, 0x05)  // _UID: Unique ID
        }
#endif

#if 0
        Device (EHC0)
        {
            Name (_HID, "PNP0D20")      // _HID: Hardware ID
            Name (_UID, 0x00)           // _UID: Unique ID
            Name (_CCA, 0x00)           // Not coherent!

            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xfe380000,         // Address Base (MMIO)
                    0x00040000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                {
                    0x0000003A,
                }
            })
        }

        Device (EHC1)
        {
            Name (_HID, "PNP0D20")
            Name (_UID, 0x01)           // _UID: Unique ID
            Name (_CCA, 0x00)

            Name(_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xfe3c0000,         // Address Base (MMIO)
                    0x00040000,         // Address Length
                )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                {
                    0x0000003E,
                }
            })
        }
#endif

        Device (USB0) {    //USB0
            Name ( _ADR, 0xfe380000)  // _ADR: Address
            Name (_CID, "ACPI\\PNP0D20")  // _CID: Compatible ID
            Name (_HRV, 0x00)  // _HRV: Hardware Revision
            Name (_UID, Zero)  // _UID: Unique ID
            Method (_HID, 0, NotSerialized)  // _HID: Hardware ID
            {
                Return ("PNP0D20")
            }
            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
            {
                Store (0xfe380000, Local0)
                Store (0x20000, Local1)
                Return (UCRS (Local0, Local1, 58, 0x03))
            }
        }
        
        Device (USB1) {    //USB1
            Name ( _ADR, 0xfe3c0000)  // _ADR: Address
            Name (_CID, "ACPI\\PNP0D20")  // _CID: Compatible ID
            Name (_HRV, 0x00)  // _HRV: Hardware Revision
            Name (_UID, One)  // _UID: Unique ID
            Method (_HID, 0, NotSerialized)  // _HID: Hardware ID
            {
                Return ("PNP0D20")
            }
            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
            {
                Store (0xfe3c0000, Local0)
                Store (0x20000, Local1)
                Return (UCRS (Local0, Local1, 62, 0x03))
            }
        }

        Device (COM1)
        {
            Name (_HID, "RKCP8250")  // _HID: Hardware ID
            Name (_CID, Package (0x02)  // _CID: Compatible ID
            {
                "HISI0031",
                "8250dw"
            })
            Name (_ADR, 0xFF1A0000)  // _ADR: Address
            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xFF1A0000,         // Address Base
                    0x00010000,         // Address Length
                    )
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                {
                    0x00000084,
                }
            })
            Name (_DSD, Package () {
                ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
                Package () {
                    Package () {"clock-frequency", 24000000},
                }
            })
        }
    }
}
