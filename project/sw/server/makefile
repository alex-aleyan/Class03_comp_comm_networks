#use this compiler:
CC=gcc
CC_OPTIONS=-v -Wall -W -D_GNU_SOURCE -DMAX_NUM_OF_FILES=10 -DMAX_NUM_OF_LINES=1024 -DRECEIVE_BUFFER_SIZE=9000
CC_DEBUG_OPTIONS=-v -g --save-temps -Wall -W -D_GNU_SOURCE -W -DGETPROTOBYNUMBER
PROG_NAME=server
TARGET=$(PROG_NAME).run
SRC=$(PROG_NAME).c

#Create macro for header files' incude directory
#H_INCLUDES=./../../include
#Implements next wildcard: ./mylib/includes/h_files/*c
#H_SOURCE=$(wildcard $(H_INCLUDES)/*.h)


all: non_debug
alldebug: debug

debug:
	$(CC) $(CC_DEBUG_OPTIONS) $(SRC) -o $(TARGET)


non_debug:
	$(CC) $(CC_OPTIONS) $(SRC) -o $(TARGET)
  

clean : 
#rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
	rm -f *.run *.i *.o *.s *.core dump*
