#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 Advanced Micro Devices, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#
AGESA_ROOT ?= src/vendorcode/amd/agesa/f15

AGESA_INC := -I$(AGESA_ROOT)/ \
             -I$(AGESA_ROOT)/Include \
             -I$(AGESA_ROOT)/Proc/IDS/ \
             -I$(AGESA_ROOT)/Proc/CPU/ \
             -I$(AGESA_ROOT)/Proc/CPU/Family

CFLAGS += $(AGESA_INC)

romstage-y += reset.c
romstage-y += buildOpts.c
romstage-y += agesawrapper.c
romstage-y += dimmSpd.c
romstage-y += BiosCallOuts.c
romstage-y += PlatformGnbPcie.c

ramstage-y += reset.c
ramstage-y += buildOpts.c
ramstage-y += agesawrapper.c
ramstage-y += dimmSpd.c
ramstage-y += BiosCallOuts.c
ramstage-y += PlatformGnbPcie.c

