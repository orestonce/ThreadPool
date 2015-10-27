a.out	: main.cpp ThreadPool.cpp ThreadPool.h
	g++ -std=c++11 -lpthread -Wall $^

run	: a.out
	./a.out
