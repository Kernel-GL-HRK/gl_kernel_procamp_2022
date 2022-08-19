KERNELDIR ?= /home/spiatakov/work/cources/kernel/tools/buildroot-2022.02.4/output/build/linux-5.15.18/

obj-m := hello.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(CC) -static main.c -o main

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	$(RM) main

