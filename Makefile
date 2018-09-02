CFLAGS=-g -Wall

wait4ports: main.o list.o
	$(CC) $(CFLAGS) -o $@ $^

list.o: list.c list.h util.h
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	mkdir -p $(PREFIX)/usr/bin
	install -m 0755 wait4ports $(PREFIX)/usr/bin/wait4ports

clean:
	rm -f *.o wait4ports
