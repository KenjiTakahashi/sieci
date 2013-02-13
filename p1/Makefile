CFLAGS= -Wall -std=c99 -pedantic -std=c99
LFLAGS= -lm
NAME=traceroute
SRC=p1.c icmp.c icmp.h
OBJS=p1.o icmp.o
YOU: $(SRC) $(NAME)
$(NAME):$(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)

p1.o: p1.c
	gcc -c $(CFLAGS) p1.c
icmp.o: icmp.c
	gcc -c $(CFLAGS) icmp.c
