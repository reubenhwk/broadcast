
.PHONY: clean all

all: bclient bserver

bclient: bclient.c
	gcc -o bclient bclient.c -std=gnu99 -lrt

bserver: bserver.c
	gcc -o bserver bserver.c -std=gnu99

clean:
	rm -f bclient bserver
