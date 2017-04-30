# Makefile for creating C program / Arduino code

FLOW := C

###################### ARDUINO ######################
ifeq ($(FLOW),ARD)
#ARDUINO_LIBS = Wire
MONITOR_BAUDRATE = 115200 

include /usr/share/arduino/Arduino.mk

else 

###################### DragonBoard ######################
#Libs
PROGRAM = exe_DB_for_arduino
_OBJ = \
	squareMraa.o
#	squareWave.o

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

#Flags
#CC = gcc
CC = g++
#CPPFLAGS = -g -Wall -pedantic
LIBS = -lsoc -lmraa

ODIR=obj
DB_DIR = db_src
CFLAGS=-I$(DB_DIR)

RM = rm -rf

# making all progs
all: main run
complete: clean main run

#compiling each c file to obj
$(ODIR)/%.o: $(DB_DIR)/%.c $(ODIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(LIBS)

# making exe from objs
main: $(OBJ)
	$(CC) -o $(PROGRAM) $(OBJ) $(CFLAGS) $(LIBS)

run: main
	sudo ./$(PROGRAM)


# prepare dir
$(ODIR):
	mkdir -p $(ODIR)


#cleaning
clean:
	$(RM) $(ODIR) *.bak *~* "#"* core main flat *.cpp.* *.h.* $(PROGRAM)

endif
