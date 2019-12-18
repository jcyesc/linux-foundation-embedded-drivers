
# Note: in order to execute this Makefile, the linaro cross compiler
# has to be in the path. Another option is to set the CROSS_COMPILE
# environment variable and then we refer to this variable in this
# file as $(CROSS_COMPILE). 

obj-m += lab_char_module1.o lab_char_module2.o


KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNEL_DIR) \
		M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) \
		M=$(PWD) clean

