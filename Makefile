# Define loadable kernel module 'ldd.o' to be built from ldd.c source file
# obj-m indicates this is a loadable module build target
obj-m += ldd.o

# KDIR points to kernel build directory, defaults to current kernel version's build dir
KDIR ?= /lib/modules/$(shell uname -r)/build
# PWD stores current working directory path
PWD  := $(shell pwd)

# Build kernel module from ldd.c source file
# KDIR should be set to the kernel build directory
# PWD represents the current working directory
all: ldd.c
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean