# Makefile2.6
ifneq ($(KERNELRELEASE),)
#kbuild syntax. dependency relationship of files and target modules are listed here.
obj-m := char_dev.o
#module-objs := file1.o file2.o

else
PWD := $(shell pwd)
KVER = 2.6.35.3
KDIR := ~/linux-2.6.35.3

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp *.ko ~/nfsroot/

clean:
	rm -rf *.cmd *.o *.mod.c *.ko .tmp_versions *odule*

endif
