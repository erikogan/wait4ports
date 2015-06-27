CFLAGS=-g

wait4ports: main.o
	$(CC) -o $@ $^

install:
	mkdir -p $(PREFIX)/usr/bin
	install -m 0755 wait4ports $(PREFIX)/usr/bin/wait4ports

clean:
	rm -f *.o wait4ports
