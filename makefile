all:	redimpl	aqmimpl

redimpl.o:	RED.c	Red.h
	gcc	-c	RED.c	-o	redimpl.o

redimpl:	redimpl.o
	gcc	redimpl.o	-o	redimpl

aqmimpl.o:	AQMCallback.c	Red.h
	gcc	-c	AQMCallback.c	-o	aqmimpl.o

aqmimpl:	aqmimpl.o
	gcc	aqmimpl.o	-o	aqmimpl

clean:
	-rm	-f	redimpl.o
	-rm	-f	redimpl
	-rm	-f	aqmimpl.o
	-rm	-f	aqmimpl
