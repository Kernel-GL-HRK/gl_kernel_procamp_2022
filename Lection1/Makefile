KERNELDIR ?= ../../buildroot-2022.02.4/output/build/linux-5.15.18/

obj-m := test-mod.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean