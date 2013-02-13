CFLAGS= -Wall -pedantic -std=gnu99
LFLAGS= -lm
NAME=whois-proxy
SRC=main.c main.h sockwrap.c sockwrap.h tcpconn.c tcpconn.h readchunk.c readchunk.h cache.c cache.h
OBJS=main.o sockwrap.o tcpconn.o readchunk.o cache.o
YOU: $(SRC) $(NAME)
$(NAME):$(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)

main.o: main.c
	gcc -c $(CFLAGS) main.c
sockwrap.o: sockwrap.c
	gcc -c $(CFLAGS) sockwrap.c
tcpconn.o: tcpconn.c
	gcc -c $(CFLAGS) tcpconn.c
readchunk.o: readchunk.c
	gcc -c $(CFLAGS) readchunk.c
cache.o: cache.c
	gcc -c $(CFLAGS) cache.c
	
clean:
	rm *.o
	rm whois-proxy
