# Module Name
obj-m += mod_klm.o

#Add src file to build mod_klm
mod_klm-y := mod_init.o

BUILD_DIR ?= $(shell pwd)/build
BUILD_DIR_MAKEFILE ?= $(PWD)/build/Makefile

KDIR ?= /lib/modules/$(shell uname -r)/build


all: $(BUILD_DIR_MAKEFILE)
	make -C $(KDIR)  M=$(BUILD_DIR) src=$(PWD) modules

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	touch "$@"

$(BUILD_DIR):
	mkdir -p "$@"

clean:
	make -C $(KDIR)  M=$(BUILD_DIR) src=$(PWD) clean
