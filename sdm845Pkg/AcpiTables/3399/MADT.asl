/*
 * Copyright (c) 2020, Xilin Wu <strongtz@yeah.net>
 *
 * ACPI Data Table [APIC]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue
 */

[000h 0000   4]                    Signature : "APIC"    [Multiple APIC Description Table (MADT)]
[004h 0004   4]                 Table Length : 000002ED
[008h 0008   1]                     Revision : 05
[009h 0009   1]                     Checksum : 0C
[00Ah 0010   6]                       Oem ID : "RKCP  "
[010h 0016   8]                 Oem Table ID : "RKCPEDK2"
[018h 0024   4]                 Oem Revision : 00003399
[01Ch 0028   4]              Asl Compiler ID : "FUCK"
[020h 0032   4]        Asl Compiler Revision : 00000001

[024h 0036   4]           Local Apic Address : 00000000
[028h 0040   4]        Flags (decoded below) : 00000000
                         PC-AT Compatibility : 0

[02Ch 0044   1]                Subtable Type : 0B [Generic Interrupt Controller]
[02Dh 0045   1]                       Length : 50
[02Eh 0046   2]                     Reserved : 0000
[030h 0048   4]         CPU Interface Number : 00000000
[034h 0052   4]                Processor UID : 00000000
[038h 0056   4]        Flags (decoded below) : 00000001
                           Processor Enabled : 1
          Performance Interrupt Trigger Mode : 0
          Virtual GIC Interrupt Trigger Mode : 0
[03Ch 0060   4]     Parking Protocol Version : 00000000
[040h 0064   4]        Performance Interrupt : 00000023
[044h 0068   8]               Parked Address : 0000000000000000
[04Ch 0076   8]                 Base Address : 00000000FFF00000
[054h 0084   8]     Virtual GIC Base Address : 00000000FFF20000
[05Ch 0092   8]  Hypervisor GIC Base Address : 00000000FFF10000
[064h 0100   4]        Virtual GIC Interrupt : 00000025
[068h 0104   8]   Redistributor Base Address : 0000000000000000
[070h 0112   8]                    ARM MPIDR : 0000000000000000
[078h 0120   1]             Efficiency Class : 00
[079h 0121   3]                     Reserved : 000000
                      SPE Overflow Interrupt : 0000
/*
[07Ch 0124   1]                Subtable Type : 0B [Generic Interrupt Controller]
[07Dh 0125   1]                       Length : 50
[07Eh 0126   2]                     Reserved : 0000
[080h 0128   4]         CPU Interface Number : 00000001
[084h 0132   4]                Processor UID : 00000001
[088h 0136   4]        Flags (decoded below) : 00000001
                           Processor Enabled : 1
          Performance Interrupt Trigger Mode : 0
          Virtual GIC Interrupt Trigger Mode : 0
[08Ch 0140   4]     Parking Protocol Version : 00000000
[090h 0144   4]        Performance Interrupt : 00000023
[094h 0148   8]               Parked Address : 0000000000000000
[09Ch 0156   8]                 Base Address : 00000000FFF00000
[0A4h 0164   8]     Virtual GIC Base Address : 00000000FFF20000
[0ACh 0172   8]  Hypervisor GIC Base Address : 00000000FFF10000
[0B4h 0180   4]        Virtual GIC Interrupt : 00000025
[0B8h 0184   8]   Redistributor Base Address : 0000000000000000
[0C0h 0192   8]                    ARM MPIDR : 0000000000000001
[0C8h 0200   1]             Efficiency Class : 00
[0C9h 0201   3]                     Reserved : 000000
                      SPE Overflow Interrupt : 0000

[0CCh 0204   1]                Subtable Type : 0B [Generic Interrupt Controller]
[0CDh 0205   1]                       Length : 50
[0CEh 0206   2]                     Reserved : 0000
[0D0h 0208   4]         CPU Interface Number : 00000002
[0D4h 0212   4]                Processor UID : 00000002
[0D8h 0216   4]        Flags (decoded below) : 00000001
                           Processor Enabled : 1
          Performance Interrupt Trigger Mode : 0
          Virtual GIC Interrupt Trigger Mode : 0
[0DCh 0220   4]     Parking Protocol Version : 00000000
[0E0h 0224   4]        Performance Interrupt : 00000023
[0E4h 0228   8]               Parked Address : 0000000000000000
[0ECh 0236   8]                 Base Address : 00000000FFF00000
[0F4h 0244   8]     Virtual GIC Base Address : 00000000FFF20000
[0FCh 0252   8]  Hypervisor GIC Base Address : 00000000FFF10000
[104h 0260   4]        Virtual GIC Interrupt : 00000025
[108h 0264   8]   Redistributor Base Address : 0000000000000000
[110h 0272   8]                    ARM MPIDR : 0000000000000002
[118h 0280   1]             Efficiency Class : 00
[119h 0281   3]                     Reserved : 000000
                      SPE Overflow Interrupt : 0000

[11Ch 0284   1]                Subtable Type : 0B [Generic Interrupt Controller]
[11Dh 0285   1]                       Length : 50
[11Eh 0286   2]                     Reserved : 0000
[120h 0288   4]         CPU Interface Number : 00000003
[124h 0292   4]                Processor UID : 00000003
[128h 0296   4]        Flags (decoded below) : 00000001
                           Processor Enabled : 1
          Performance Interrupt Trigger Mode : 0
          Virtual GIC Interrupt Trigger Mode : 0
[12Ch 0300   4]     Parking Protocol Version : 00000000
[130h 0304   4]        Performance Interrupt : 00000023
[134h 0308   8]               Parked Address : 0000000000000000
[13Ch 0316   8]                 Base Address : 00000000FFF00000
[144h 0324   8]     Virtual GIC Base Address : 00000000FFF20000
[14Ch 0332   8]  Hypervisor GIC Base Address : 00000000FFF10000
[154h 0340   4]        Virtual GIC Interrupt : 00000025
[158h 0344   8]   Redistributor Base Address : 0000000000000000
[160h 0352   8]                    ARM MPIDR : 0000000000000003
[168h 0360   1]             Efficiency Class : 00
[169h 0361   3]                     Reserved : 000000
                      SPE Overflow Interrupt : 0000

[16Ch 0364   1]                Subtable Type : 0B [Generic Interrupt Controller]
[16Dh 0365   1]                       Length : 50
[16Eh 0366   2]                     Reserved : 0000
[170h 0368   4]         CPU Interface Number : 00000004
[174h 0372   4]                Processor UID : 00000004
[178h 0376   4]        Flags (decoded below) : 00000001
                           Processor Enabled : 1
          Performance Interrupt Trigger Mode : 0
          Virtual GIC Interrupt Trigger Mode : 0
[17Ch 0380   4]     Parking Protocol Version : 00000000
[180h 0384   4]        Performance Interrupt : 00000023
[184h 0388   8]               Parked Address : 0000000000000000
[18Ch 0396   8]                 Base Address : 00000000FFF00000
[194h 0404   8]     Virtual GIC Base Address : 00000000FFF20000
[19Ch 0412   8]  Hypervisor GIC Base Address : 00000000FFF10000
[1A4h 0420   4]        Virtual GIC Interrupt : 00000025
[1A8h 0424   8]   Redistributor Base Address : 0000000000000000
[1B0h 0432   8]                    ARM MPIDR : 0000000000000100
[1B8h 0440   1]             Efficiency Class : 01
[1B9h 0441   3]                     Reserved : 000000
                      SPE Overflow Interrupt : 0000

[1BCh 0444   1]                Subtable Type : 0B [Generic Interrupt Controller]
[1BDh 0445   1]                       Length : 50
[1BEh 0446   2]                     Reserved : 0000
[1C0h 0448   4]         CPU Interface Number : 00000005
[1C4h 0452   4]                Processor UID : 00000005
[1C8h 0456   4]        Flags (decoded below) : 00000001
                           Processor Enabled : 1
          Performance Interrupt Trigger Mode : 0
          Virtual GIC Interrupt Trigger Mode : 0
[1CCh 0460   4]     Parking Protocol Version : 00000000
[1D0h 0464   4]        Performance Interrupt : 00000023
[1D4h 0468   8]               Parked Address : 0000000000000000
[1DCh 0476   8]                 Base Address : 00000000FFF00000
[1E4h 0484   8]     Virtual GIC Base Address : 00000000FFF20000
[1ECh 0492   8]  Hypervisor GIC Base Address : 00000000FFF10000
[1F4h 0500   4]        Virtual GIC Interrupt : 00000025
[1F8h 0504   8]   Redistributor Base Address : 0000000000000000
[200h 0512   8]                    ARM MPIDR : 0000000000000101
[208h 0520   1]             Efficiency Class : 01
[209h 0521   3]                     Reserved : 000000
                      SPE Overflow Interrupt : 0000
*/
[2ACh 0684   1]                Subtable Type : 0C [Generic Interrupt Distributor]
[2ADh 0685   1]                       Length : 19
[2AEh 0686   2]                     Reserved : 0000
[2B0h 0688   4]        Local GIC Hardware ID : 00000000
[2B4h 0692   8]                 Base Address : 00000000FEE00000
[2BCh 0700   4]               Interrupt Base : 00000000
[2C0h 0704   1]                      Version : 03
[2C1h 0705   3]                     Reserved : 000000

[2C5h 0709   1]                Subtable Type : 0E [Generic Interrupt Redistributor]
[2C6h 0710   1]                       Length : 10
[2C7h 0711   2]                     Reserved : 0000
[2C9h 0713   8]                 Base Address : 00000000FEF00000
[2D1h 0721   4]                       Length : 000C0000

[2D5h 0725   1]                Subtable Type : 0F [GIC ITS]
[2D6h 0726   1]                       Length : 20
[2D7h 0727   2]                     Reserved : 0000
[2D9h 0729   4]               Translation ID : 00000000
[2DDh 0733   8]                 Base Address : 00000000FEE20000
[2E5h 0741   4]                     Reserved : 00000000
