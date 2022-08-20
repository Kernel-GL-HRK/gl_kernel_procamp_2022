KERNELDIR ?= /home/spiatakov/work/cources/kernel/tools/buildroot-2022.02.4/output/build/linux-5.15.18/

obj-m := hello.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(CXX) -static -std=c++14 main.cpp -o main -Wall -Wextra

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	$(RM) main

