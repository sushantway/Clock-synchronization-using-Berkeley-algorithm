all: time process clean

time: time.cpp
	gcc time.cpp -o time.out -lpthread

process: process.cpp
	gcc process.cpp -o process.out -lpthread

clean:
	rm -rf *o
