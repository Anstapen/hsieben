TOPTARGETS := all clean flash

.DEFAULT_GOAL = all

SUBDIRS := $(wildcard CM*/.)
GDB = gdb-multiarch
CM4_ARGS =
CM7_ARGS = 

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

debug-cm4:
	$(GDB) -x cm4_debug.cfg CM4/build/CM4_Firmware.elf

debug-cm7:
	$(GDB) -x cm7_debug.cfg CM7/build/CM7_Firmware.elf

gdbserver:
	openocd -f openocd_h7_dual.cfg


.PHONY: $(TOPTARGETS) $(SUBDIRS) debug-cm4 debug-cm7 gdbserver