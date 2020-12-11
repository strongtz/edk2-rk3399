/** @file

  Commonly included header.

  Copyright (c) 2017, Linaro Ltd. All rights reserved.<BR>
  Copyright (C) 2018, Marvell International Ltd. and its affiliates.<BR>
  Copyright (c) 2019, Andrey Warkentin <andrey.warkentin@gmail.com>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <IndustryStandard/Acpi.h>

#define ACPI_OEM_ID_ARRAY        {'R','K','C','P',' ',' '}
#define ACPI_OEM_REVISION        0
#define ACPI_CREATOR_ID          SIGNATURE_32('E','D','K','2')
#define ACPI_CREATOR_REVISION    0

#define ACPI_OEM_TABLE_ID        SIGNATURE_64('R','K','3','3','9','9',' ',' ')


/**
 * A macro to initialize the common header part of EFI ACPI tables
 * as defined by EFI_ACPI_DESCRIPTION_HEADER structure.
 **/
#define __ACPI_HEADER(sign, type, rev) {                \
  sign,                   /* UINT32  Signature */       \
  sizeof (type),          /* UINT32  Length */          \
  rev,                    /* UINT8   Revision */        \
  0,                      /* UINT8   Checksum */        \
  ACPI_OEM_ID_ARRAY,      /* UINT8   OemId[6] */        \
  ACPI_OEM_TABLE_ID,      /* UINT64  OemTableId */      \
  ACPI_OEM_REVISION,      /* UINT32  OemRevision */     \
  ACPI_CREATOR_ID,        /* UINT32  CreatorId */       \
  ACPI_CREATOR_REVISION   /* UINT32  CreatorRevision */ \
  }
