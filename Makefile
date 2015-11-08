obj-m	+=	aqmmodule.o
aqmmodule-objs	:=	AQMCallback.o	RED.o
KDIR	:=	/lib/modules/$(shell uname -r)/build
pwd	:=	$(shell pwd)
default:
	make	-C	$(KDIR)	M=$(pwd)	modules
clean:
	make	-C	$(KDIR)	M=$(pwd)	clean

