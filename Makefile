CC = g++
CFLAGS = -g -Wall -pedantic -Wmaybe-uninitialized



all:	resourceanalyzer	/

resourceanalyzer: resourceanalyzer.o
	$(CC) $(CFLAGS) -o $@ resourceanalyzer.o

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f resourceanalyzer
	rm -f *.o
