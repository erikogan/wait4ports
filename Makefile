CFLAGS=-g -Wall

wait4ports: main.o list.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c util.h version.h

list.o: list.c list.h util.h
#	$(CC) $(CFLAGS) -c -o $@ $<

install:
	mkdir -p $(PREFIX)/usr/bin
	install -m 0755 wait4ports $(PREFIX)/usr/bin/wait4ports

check:
	sh -c './wait4ports -q -t 1 -s 1 tcp://127.0.0.1:65535 ; status=$$? ; [ $$status -eq 255 ] || [ $$status -eq 0 ]'

clean:
	rm -f *.o wait4ports
