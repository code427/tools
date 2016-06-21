CONFIG_NAND_SPL	= y

include $(TOPDIR)/config.mk

PAD_TO :=

nandobj	:= $(OBJTREE)/nand_spl/

LDSCRIPT= $(TOPDIR)/nand_spl/board/$(BOARDDIR)/u-boot.lds
LSTSCRIPT= $(nandobj)/board/$(BOARDDIR)/u-boot.lst
LDFLAGS := -T $(nandobj)u-boot.lds -Ttext $(CONFIG_SPL_TEXT_BASE) $(LDFLAGS) \
	   $(LDFLAGS_FINAL)
AFLAGS	+= -DCONFIG_SPL_BUILD -DCONFIG_NAND_SPL 
CFLAGS	+= -DCONFIG_SPL_BUILD -DCONFIG_NAND_SPL

SOBJS	= spl_start.o crt0.o lowlevel_init.o dcdheader.o
COBJS	= nand_boot_fsl_nfc.o srk.o

OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS)
LNDIR	:= $(nandobj)board/$(BOARDDIR)

SYSTEM_MAP = \
		$(NM) $1 | \
		grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | \
		LC_ALL=C sort

ALL	= $(nandobj)u-boot-spl $(nandobj)u-boot-spl.bin $(nandobj)u-boot-spl-64k.bin

RST_START := $(shell ls $(nandobj) -r | grep "uboot-spl*" | sed -e '2,$$d' -e 's/uboot-spl//g' -e 's/.bin//g')

RST_TEMP := $(shell echo $(shell expr $(RST_START) + 1) | wc -c)

RST := $(shell if [ "$(RST_TEMP)" = "3" ]; then  echo "$(shell expr $(RST_START) + 1)" | sed 's/^/0&/g'; else if [ "$(RST_TEMP)" = "2" ]; then  echo "$(shell expr $(RST_START) + 1)" | sed 's/^/00&/g'; else if [ "$(RST_TEMP)" = "4" ]; then  echo "$(shell expr $(RST_START) + 1)"; else echo "error"; fi; fi; fi)

all:	$(obj).depend $(ALL)

$(nandobj)u-boot-spl-64k.bin: $(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} --pad-to=$(CONFIG_SPL_PAD_TEXT) -O binary $< $@
	cp $(nandobj)u-boot-spl-64k.bin $(nandobj)uboot-spl$(RST).bin

$(nandobj)u-boot-spl.bin:	$(nandobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@
	@$(call SYSTEM_MAP,$<) > $(nandobj)/spl_system.map	

$(nandobj)u-boot-spl:	$(OBJS) $(nandobj)u-boot.lds
	cd $(LNDIR) && $(LD) $(LDFLAGS) $(__OBJS) \
		-Map $(nandobj)u-boot-spl.map \
		-o $@

# The following line expands into whole rule which generates $(LSTSCRIPT),
# the file containing u-boots LG-array linker section. This is included into
# $(LDSCRIPT). The function make_u_boot_list is defined in helper.mk file.
$(eval $(call make_u_boot_list, $(LSTSCRIPT), $(OBJS)))
$(nandobj)u-boot.lds: $(LDSCRIPT) $(LSTSCRIPT)
	$(CPP) $(CPPFLAGS) $(LDPPFLAGS) -I$(nandobj)/board/$(BOARDDIR) \
		-ansi -D__ASSEMBLY__ -P - <$< >$@

#########################################################################

$(obj)%.o:	$(SRCTREE)/nand_spl/board/freescale/mx25pdk/%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(SRCTREE)/arch/arm/lib/%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(SRCTREE)/board/freescale/mx25pdk/%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(SRCTREE)/nand_spl/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
