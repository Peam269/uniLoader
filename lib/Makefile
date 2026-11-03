lib-y += compiler-rt.o

# debug
lib-y += debug/debug.o
lib-$(CONFIG_UART_DEBUG) += debug/uart.o
lib-$(CONFIG_SIMPLE_FB) += video/font.o

# unic (neatlibc still temporarily resides)
lib-y += unic/string.o

lib-$(CONFIG_LIBFDT) += \
	libfdt/fdt.o \
	libfdt/fdt_ro.o \
	libfdt/fdt_rw.o \
	libfdt/fdt_wip.o \
	libfdt/fdt_sw.o \
	libfdt/fdt_strerror.o

lib-$(CONFIG_LIBFDT) += \
	fixfdt/patch-fdt.o \
	fixfdt/patches/memory.o \
	hisi/fdt.o \
	hisi/patches.o

ifneq ($(CONFIG_RAMDISK_NO_COPY),y)
lib-$(CONFIG_LIBFDT) += fixfdt/patches/ramdisk.o
endif

ifneq ($(wildcard $(CONFIG_CMDLINE_PATH:"%"=%)),)
ccflags-y += -DCMDLINE_STR='"$(shell cat $(CONFIG_CMDLINE_PATH) | tr -d '\n')"'
lib-$(CONFIG_LIBFDT) += fixfdt/patches/cmdline.o
endif

ccflags-y += -I$(srctree)/include/lib/libfdt
