main: grep_ck sort_ck

grep_ck: grep_ck.o 
	gcc -Wall grep_ck.o -o grep_ck
	
sort_ck: sort_ck.o
	gcc -Wall sort_ck.o -o sort_ck

grep_ck.o: grep_ck.c
	gcc -Wall -c grep_ck.c
	
sort_ck.o: sort_ck.c
	gcc -Wall -c sort_ck.c

clean:
	rm *.o
	rm grep_ck
	rm sort_ck
