/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* DefinitionBlock Statement */
DefinitionBlock (
	"DSDT.AML",	/* Output filename */
	"DSDT",		/* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	"ASUS",	/* OEMID */
	"COREBOOT",	/* TABLE ID */
	0x00010001	/* OEM Revision */
	)
{	/* Start of ASL file */
	/* #include <arch/x86/acpi/debug.asl> */	/* Include global debug methods if needed */
	
	#include "acpi/mainboard.asl"

	/*
	 * Processor Object
	 *
	 */
	Scope (\_PR) {		/* define processor scope */
		Processor(
			C000,		/* name space name, align with BLDCFG_PROCESSOR_SCOPE_NAME[01] */
			0x00,		/* Unique number for this processor */
			0x810,		/* PBLK system I/O address !hardcoded! */
			0x06		/* PBLKLEN for boot processor */
			) {
		}
		Processor(C001, 0x01, 0x00000000, 0x00) {}
		Processor(C002, 0x02, 0x00000000, 0x00) {}
		Processor(C003, 0x03, 0x00000000, 0x00) {}
		Processor(C004, 0x04, 0x00000000, 0x00) {}
		Processor(C005, 0x05, 0x00000000, 0x00) {}
		Processor(C006, 0x06, 0x00000000, 0x00) {}
		Processor(C007, 0x07, 0x00000000, 0x00) {}
		Processor(C008, 0x08, 0x00000000, 0x00) {}
		Alias (C000, CPU0)
		Alias (C001, CPU1)
		Alias (C002, CPU2)
		Alias (C003, CPU3)
		Alias (C004, CPU4)
		Alias (C005, CPU5)
		Alias (C006, CPU6)
		Alias (C007, CPU7)
		Alias (C008, CPU8)
	} /* End _PR scope */

	#include "acpi/routing.asl"
	
	Scope(\_SB) {
		/* global utility methods expected within the \_SB scope */
		#include <arch/x86/acpi/globutil.asl>

		Device(PCI0) {

			/* Describe the AMD Northbridge */
			#include <northbridge/amd/agesa/family14/acpi/northbridge.asl>

			/* Describe the AMD Fusion Controller Hub Southbridge */
			#include <southbridge/amd/cimx/sb800/acpi/fch.asl>

		}
	}   /* End Scope(_SB)  */

	/* Contains the supported sleep states for this chipset */
	#include <southbridge/amd/cimx/sb800/acpi/sleepstates.asl>

	/* Contains the Sleep methods (WAK, PTS, GTS, etc.) */
	#include "acpi/sleep.asl"

	#include "acpi/gpe.asl"
	#include <southbridge/amd/cimx/sb800/acpi/smbus.asl>
	#include "acpi/thermal.asl"
}
/* End of ASL file */
