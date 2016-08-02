ifndef KERNEL_SRC_PATH
KERNEL_SRC_PATH := /home/mvjseppa/rpi-kernel/linux/
endif

obj-m+=md25.o
CFLAGS_md25.o := -DDEBUG

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNEL_SRC_PATH) M=$(PWD) modules

clean:
	make -C $(KERNEL_SRC_PATH) M=$(PWD) clean

