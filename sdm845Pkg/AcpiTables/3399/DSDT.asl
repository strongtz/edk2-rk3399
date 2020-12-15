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

        Device (EHC0)
        {
            Name (_HID, "PNP0D20")      // _HID: Hardware ID
            Name (_UID, 0x00)           // _UID: Unique ID
            Name (_CCA, 0x00)           // Not coherent!

            Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
            {
                Memory32Fixed (ReadWrite,
                    0xfe380000,         // Address Base (MMIO)
                    0x00010000,         // Address Length
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
            Name (_CID, "HISI0D2")      // _HID: Hardware ID
            Name (_UID, 0x01)           // _UID: Unique ID
            Name (_CCA, One)

            Method(_CRS, 0x0, Serialized){
                Name (RBUF, ResourceTemplate ()  // _CRS: Current Resource Settings
                {
                    Memory32Fixed (ReadWrite,
                        0xfe3c0000,         // Address Base (MMIO)
                        0x00000400,         // Address Length
                    )
                    Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, )
                    {
                        0x0000003E,
                    }
                })
                Return(RBUF)
            }

            //
            // Root Hub
            //
            Device (RHUB){
                Name(_ADR, 0x00000000)  // Address of Root Hub should be 0 as per ACPI 5.0 spec

                Device(PRT0){
                    Name(_ADR, 0x00000000)
                    Name(_UPC, Package(){
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name(_PLD, Package(){
                        Buffer(0x10){
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT1
                Device(PRT1){
                    Name(_ADR, 0x00000001)
                    Name(_UPC, Package(){
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name(_PLD, Package(){
                        Buffer(0x10){
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT2

                Device(PRT2){
                    Name(_ADR, 0x00000002)
                    Name(_UPC, Package(){
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name(_PLD, Package(){
                        Buffer(0x10){
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT3

                Device(PRT3){
                    Name(_ADR, 0x00000003)
                    Name(_UPC, Package(){
                        0xFF,        // Port is connectable
                        0x00,        // Port connector is A
                        0x00000000,
                        0x00000000
                    })
                    Name(_PLD, Package(){
                        Buffer(0x10){
                            0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x31, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                        }
                    })
                } // USB0_RHUB_HUB1_PRT4
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
                    0x00000100,         // Address Length
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
