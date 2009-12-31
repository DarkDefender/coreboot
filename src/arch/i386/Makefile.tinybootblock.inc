ifdef POST_EVALUATION

#######################################################################
# Build the final rom image

$(obj)/coreboot.pre1: $(obj)/coreboot.bootblock $(CBFSTOOL)
	rm -f $@
	$(CBFSTOOL) $@ create $(CONFIG_COREBOOT_ROMSIZE_KB)K $(obj)/coreboot.bootblock

$(obj)/coreboot.pre: $(obj)/coreboot.romstage $(obj)/coreboot.pre1 $(CBFSTOOL)
	rm -f $@
	cp $(obj)/coreboot.pre1 $@
	$(CBFSTOOL) $@ add-stage $(obj)/romstage.elf fallback/romstage x 0x$(shell cat $(obj)/location.txt)
#FIXME: location.txt might require an offset of header size

#######################################################################
# Build the bootblock

$(obj)/coreboot.bootblock: $(obj)/bootblock.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY) -O binary $< $@

bootblock_lds := $(src)/arch/i386/init/ldscript_failover.lb
bootblock_lds += $(src)/cpu/x86/16bit/entry16.lds
bootblock_lds += $(src)/cpu/x86/16bit/reset16.lds
bootblock_lds += $(src)/arch/i386/lib/id.lds

bootblock_inc := $(src)/arch/i386/init/bootblock_prologue.c
bootblock_inc += $(src)/cpu/x86/16bit/entry16.inc
bootblock_inc += $(src)/cpu/x86/16bit/reset16.inc
bootblock_inc += $(src)/cpu/x86/32bit/entry32.inc
bootblock_inc += $(src)/arch/i386/lib/id.inc
ifeq ($(CONFIG_SSE),y)
bootblock_inc += $(src)/cpu/x86/sse_enable.inc
endif
bootblock_inc += $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.inc
bootblock_inc += $(src)/arch/i386/lib/walkcbfs.S

bootblock_romccflags := -mcpu=i386
ifeq ($(CONFIG_SSE),y)
bootblock_romccflags := -mcpu=k7 -msse
endif

$(obj)/bootblock/ldscript.ld: $(bootblock_ldscripts) $(obj)/ldoptions
	mkdir -p $(obj)/bootblock
	printf '$(foreach ldscript,ldoptions $(bootblock_lds),INCLUDE "$(ldscript)"\n)' > $@

$(obj)/bootblock/bootblock.c: $(bootblock_inc)
	mkdir -p $(obj)/bootblock
	printf '$(foreach crt0,config.h $(bootblock_inc),#include "$(crt0)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/bootblock.o: $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.s
	$(CC) -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/bootblock.s: $(obj)/bootblock/bootblock.c
	$(CC) -x assembler-with-cpp -DASSEMBLY -E -I$(src)/include -I$(src)/arch/i386/include -I$(obj) -I$(obj)/bootblock -include $(obj)/config.h -I. -I$(src) $< > $@.new && mv $@.new $@

$(obj)/mainboard/$(MAINBOARDDIR)/bootblock.inc: $(obj)/romcc $(src)/arch/i386/init/bootblock.c
	$(obj)/romcc $(bootblock_romccflags) -O2 $(ROMCCFLAGS) $(INCLUDES) $(src)/arch/i386/init/bootblock.c -o $@

$(obj)/bootblock.elf: $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.o $(obj)/bootblock/ldscript.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(obj)/bootblock/ldscript.ld $<
	$(NM) -n $(obj)/bootblock.elf | sort > $(obj)/bootblock.map

#######################################################################
# Build the romstage
$(obj)/coreboot.romstage: $(obj)/coreboot.pre1 $(initobjs) $(obj)/romstage/ldscript.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	printf "CONFIG_ROMBASE = 0x0;\n" > $(obj)/location.ld
	$(CC) -nostdlib -nostartfiles -static -o $(obj)/romstage.elf -L$(obj) -T $(obj)/romstage/ldscript.ld $(initobjs)
	$(OBJCOPY) -O binary $(obj)/romstage.elf $(obj)/romstage.bin
	printf "CONFIG_ROMBASE = 0x" > $(obj)/location.ld
	$(CBFSTOOL) $(obj)/coreboot.pre1 locate $(obj)/romstage.bin fallback/romstage $(CONFIG_XIP_ROM_SIZE) > $(obj)/location.txt
	cat $(obj)/location.txt >> $(obj)/location.ld
	printf ";\n" >> $(obj)/location.ld
	$(CC) -nostdlib -nostartfiles -static -o $(obj)/romstage.elf -L$(obj) -T $(obj)/romstage/ldscript.ld $(initobjs)
	$(NM) -n $(obj)/romstage.elf | sort > $(obj)/romstage.map
	$(OBJCOPY) -O binary $(obj)/romstage.elf $@

$(obj)/romstage/ldscript.ld: $(ldscripts) $(obj)/ldoptions
	mkdir -p $(obj)/romstage
	printf '$(foreach ldscript,ldoptions location.ld $(ldscripts),INCLUDE "$(ldscript:$(obj)/%=%)"\n)' > $@

$(obj)/romstage/crt0_includes.h: $(crt0s)
	mkdir -p $(obj)/romstage
	printf '$(foreach crt0,config.h $(crt0s),#include "$(crt0:$(obj)/%=%)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.o: $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s
	$(CC) -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.s: $(src)/arch/i386/init/crt0.S.lb $(obj)/romstage/crt0_includes.h
	$(CC) -x assembler-with-cpp -DASSEMBLY -E -I$(src)/include -I$(src)/arch/i386/include -I$(obj) -I$(obj)/romstage -include $(obj)/config.h -I. -I$(src) $< > $@.new && mv $@.new $@

endif