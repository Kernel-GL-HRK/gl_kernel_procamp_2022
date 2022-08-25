KERNEL_DIR ?= /mnt/nvme0n1p5/Linux_Course/buildroot/output/build/linux-5.15.18/

obj-m := Task1.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
