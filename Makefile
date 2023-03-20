CC=gcc
CFLAGS=-g -c -Wall 

all: main

main: main.o hash.o lcc.o git.o  wf.o commit.o
	$(CC) main.o hash.o lcc.o git.o wf.o commit.o -o main

main.o: main.c 
	$(CC) $(CFLAGS) main.c

hash.o: hash.c hash.h #TME1 EXO1: Prise en main du langage Bash
	$(CC) $(CFLAGS) hash.c

lcc.o: lcc.c lcc.h	#TME1 EXO2: Implémentation d’une liste de chaı̂nes de caractères
	$(CC) $(CFLAGS) lcc.c

git.o: git.c git.h	#TME1 EXO3: Gestion de fichiers sous git
	$(CC) $(CFLAGS) git.c

wf.o: wf.c wf.h	#TME2 EXO4: Fonctions de manipulation de base
	$(CC) $(CFLAGS) wf.c

commit.o: commit.c commit.h #TME3 Gestion des commits
	$(CC) $(CFLAGS) commit.c
clean:
	rm -rf *o main