Let me explain the `make` flags in your Makefile:

### -C flag
The `-C` flag tells `make` to change to the specified directory before executing the make command. In this case, `$(KDIR)` represents the kernel build directory.

Think of it like:
```bash
cd $(KDIR)
# then execute make
```

### M flag
The `M` flag is specific to kernel module building and specifies the directory where the actual module source resides. In this case, `$(PWD)` represents your current working directory where your module source code (ldd.c) is located.

This combination is a standard pattern for building Linux kernel modules where:
1. `-C $(KDIR)` switches to the kernel source directory to access its build system
2. `M=$(PWD)` tells the kernel build system where to find your module's source code

Here's a more detailed example:
````makefile
# If your kernel source is in /usr/src/linux-5.4.0
KDIR := /usr/src/linux-5.4.0
PWD := $(shell pwd)

all: ldd.c
	$(MAKE) -C $(KDIR) M=$(PWD) modules
````

The command effectively says: "Go to the kernel directory, but build the module that's in my current directory using the kernel's build system."

This setup allows your module to be built using the kernel's configuration and ensures compatibility with the target kernel version.