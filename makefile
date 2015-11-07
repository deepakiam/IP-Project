obj-m	+=	aqmmodule.o
aqmmodule-y	:=	aqmimpl.o	redimpl.o
KDIR	:=	/lib/modules/$(shell uname -r)/build
pwd	:=	$(shell pwd)

redimpl.o:	RED.c	Red.h
	gcc	-c	RED.c	-o	redimpl.o

redimpl:	redimpl.o
	gcc	redimpl.o	-o	redimpl

aqmimpl.o:	AQMCallback.c	Red.h
	gcc	-c	AQMCallback.c	-o	aqmimpl.o

aqmimpl:	aqmimpl.o
	gcc	aqmimpl.o	-o	aqmimpl
default:
	$(MAKE)	-C	$(KDIR)	M=$(pwd)	modules
