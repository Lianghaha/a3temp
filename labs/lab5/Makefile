testlist : testlist.o list.o test1.o test2.o
	gcc -Wall -g -o $@ $^

testlist.o : testlist.c 
	gcc -Wall -g -c $<

%.o : %.c list.h
	gcc -Wall -g -c $<

clean : 
	rm testlist *.o
