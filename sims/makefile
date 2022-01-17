
run:main.o op.o doubly_linklist.o viewer.o
	gcc ./controller/main.o  ./controller/op.o  ./model/doubly_linklist.o  ./viewer/viewer.o  -o run -Wall 

main.o:
	make -C ./controller/

op.o:
	make -C ./controller/

doubly_linklist.o:
	make -C ./model/

viewer.o:
	make -C ./viewer/

.PHONY:clean
clean:
	rm ./controller/main.o  ./controller/op.o  ./model/doubly_linklist.o  ./viewer/viewer.o   run
