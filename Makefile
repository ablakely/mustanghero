CC=gcc
CFLAGS=-c -I./lib -L/usr/lib/i386-linux-gnu -lusb-1.0
BINFLAGS=-L/usr/lib/i386-linux-gnu -lusb-1.0
SRC=./src
OBJ=./build
OBJECTS=$(OBJ)/*.o

main:
	@rm -rf build
	@mkdir build
	$(CC) $(CFLAGS) $(SRC)/main.c -o $(OBJ)/main.o
	$(CC) $(CFLAGS) $(SRC)/mustang.c -o $(OBJ)/mustang.o
	$(CC) $(CFLAGS) $(SRC)/amplifier.c -o $(OBJ)/amplifier.o
	$(CC) -o mustanghero $(OBJECTS) $(BINFLAGS)
	@echo "All Done!"

clean:
	@rm -rf build mustanghero
