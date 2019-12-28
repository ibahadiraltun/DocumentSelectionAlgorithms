
CC=g++
CFLAGS=-Wall

all:
	cd DocumentSelectionAlgorithms && pwd && \
	$(CC) $(CFLAGS) algo1.cpp algo12.cpp algo13.cpp algo14.cpp common.cpp hedge.cpp main.cpp ndcg.cpp -o ../algo -v

