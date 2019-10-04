#use this compiler:
CC=gcc
CC_OPTIONS=-v -Wall -W -D_GNU_SOURCE
CC_DEBUG_OPTIONS=-v -g --save-temps -Wall -W -D_GNU_SOURCE -W -DGETPROTOBYNUMBER
PROG_NAME=receiver
TARGET=$(PROG_NAME).run
SRC=$(PROG_NAME).c

#Create macro for header files' incude directory
#H_INCLUDES=./../../include
#Implements next wildcard: ./mylib/includes/h_files/*c
#H_SOURCE=$(wildcard $(H_INCLUDES)/*.h)


all: non_debug
alldebug: debug

debug:
	$(CC) $(CC_DEBUG_OPTIONS) $(SRC) ../parse_udp_args/parse_args.h -o $(TARGET)


non_debug:
	$(CC) $(CC_OPTIONS) $(SRC) -o $(TARGET)
  

clean : 
#rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
	rm -f *.run *.i *.o *.s *.core dump*
