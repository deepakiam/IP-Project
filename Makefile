obj-m	+=	aqmmodule.o
aqmmodule-objs	:=	AQMCallback.o	RED.o
KDIR	:=	/lib/modules/$(shell uname -r)/build
pwd	:=	$(shell pwd)
default:
	make	-C	$(KDIR)	M=$(pwd)	modules
clean:
	make	-C	$(KDIR)	M=$(pwd)	clean

KBUILD_EXTRA_SYMBOLS	=	/lib/modules/3.13.0-24-generic/build/Module.symvers /root/ip-project/IP-Project/Module.symvers

