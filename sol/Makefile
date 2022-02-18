CFLAGS = -Wall -lpthread
CC = g++

BIN = tema1_par

build: $(BIN)

tema1_par: tema1.cpp utils.o parallel_design.o
	$(CC) -o $@ $^ $(CFLAGS)

utils.o: utils.cpp
	$(CC) $^ -c $(CFLAGS)

parallel_design.o: parallel_design.cpp
	$(CC) $^ -c $(CFLAGS)

clean:
	rm -rf $(BIN) *.o
