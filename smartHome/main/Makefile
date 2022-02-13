arch?=arm

ifeq ($(arch), arm)
	CC := arm-linux-gnueabihf-gcc
FLAGS := -pthread -L /home/ubuntu/nfs/rootfs/lib/ -lsqlite3
endif

ifeq ($(arch), x86)
	CC := cc
FLAGS := -pthread -lsqlite3
endif

#TEST?=TEST
TRACE?=TRACE
OBJ :=main.o dev.o 

app:$(OBJ)
	$(CC) $^ -o $@ $(FLAGS) -D$(TRACE)

install:	
	cp app ~/nfs/rootfs/

.PHONY:clean
clean:
	-rm *.o app
