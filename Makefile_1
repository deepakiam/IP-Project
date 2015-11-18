obj-m	+=	addPriority.o
addP-objs	:=	addPriority.o
KDIR	:=	/lib/modules/$(shell uname -r)/build
pwd	:=	$(shell pwd)
default:
	make	-C	$(KDIR)	M=$(pwd)	modules
clean:
	make	-C	$(KDIR)	M=$(pwd)	clean

KBUILD_EXTRA_SYMBOLS	=	/lib/modules/2.6.32-21/build/Module.symvers 

