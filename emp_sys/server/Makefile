ser:server.o my_sqlite.o model.o
	gcc ./controller/server.o ./sqlite3/my_sqlite.o ./model/model.o -o ser -Wall -lsqlite3 -pthread

server.o:
	make -C ./controller/

my_sqlite.o:
	make -C ./sqlite3/

model.o:
	make -C ./model/

.PHONY:clean
clean:
	@rm ./controller/*.o ./model/*.o ./sqlite3/*.o
	@echo " 	CLEAN COMPLETE 		"
