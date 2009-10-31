CC			:=	$(CROSS_COMPILE)gcc
INCLUDES	:=	-I. \
				-I$(CROSS_COMPILE_ROOT)/usr/include/glib-2.0 \
				-I$(CROSS_COMPILE_ROOT)/usr/lib/glib-2.0/include \
				-I$(CROSS_COMPILE_ROOT)/usr/include/lunaservice \
				-I$(CROSS_COMPILE_ROOT)/usr/include/mjson \
				-Ilibircclient/include
LIBS		:= 	-lglib-2.0 -llunaservice
PROGRAM		:= 	wIRCd
OBJECTS		:= 	client.o luna_service.o main.o
ARCHIVES	:=	libircclient/src/libircclient.a

.PHONY		: 	clean-objects clean

all: $(PROGRAM)

fresh: clean all

$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(ARCHIVES) -o $(PROGRAM) $(INCLUDES) $(LIBS)

$(OBJECTS): %.o: %.c
	$(CC) -c $<  -o $@ $(INCLUDES) $(LIBS)
	
clean-objects:
	rm -rf $(OBJECTS)
	
clean: clean-objects
	rm -rf $(PROGRAM)
