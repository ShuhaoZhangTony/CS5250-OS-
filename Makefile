TARGET_MODULE:=hello_world_mod
obj-m += $(TARGET_MODULE).o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
load:
	insmod ./$(TARGET_MODULE).ko who=$(who)
unload:
	rmmod ./$(TARGET_MODULE).ko	
