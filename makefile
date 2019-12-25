
CC=g++
CFLAGS=-Wall

all:
	cd DocumentSelectionAlgorithms && pwd && \
	$(CC) $(CFLAGS) algo1.cpp common.cpp hedge.cpp main.cpp ndcg.cpp -o ../algo

