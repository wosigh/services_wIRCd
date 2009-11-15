VERSION			=	"0.0.3"

CC				=	$(CROSS_COMPILE)gcc

MAIN_INCLUDES	=	-I. \
					-I$(CROSS_COMPILE_ROOT)/usr/include/glib-2.0 \
					-I$(CROSS_COMPILE_ROOT)/usr/lib/glib-2.0/include \
					-I$(CROSS_COMPILE_ROOT)/usr/include/lunaservice \
					-I$(CROSS_COMPILE_ROOT)/usr/include/mjson \
					-Ilibircclient/include
					
ifeq ($(CS_TOOLCHAIN_ROOT),"")				
	INCLUDES	=	$(MAIN_INCLUDES)
else
	INCLUDES	=	-L$(CS_TOOLCHAIN_ROOT)/arm-none-linux-gnueabi/libc/usr/lib \
					-Xlinker -rpath-link=/opt/arm-none-linux-gnueabi/usr/lib \
					-L$(CROSS_COMPILE_ROOT)/usr/lib \
					$(MAIN_INCLUDES)
endif
					
LIBS			= 	-lglib-2.0 -llunaservice

PROGRAM			= 	us.ryanhope.wircd

OBJECTS			= 	events.o connection.o subscriptions.o client.o luna_service.o main.o

ARCHIVES		=	libircclient/src/libircclient.a

.PHONY			: 	clean-objects clean


all: $(PROGRAM)

fresh: clean all

$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(ARCHIVES) -o $(PROGRAM) $(INCLUDES) $(LIBS)

$(OBJECTS): %.o: %.c
	$(CC) -DVERSION=\"$(VERSION)\" -c $<  -o $@ -I. $(INCLUDES) $(LIBS)
	
clean-objects:
	rm -rf $(OBJECTS)
	
clean: clean-objects
	rm -rf $(PROGRAM)
