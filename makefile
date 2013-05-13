install: all
	

all: server_n client_n

server_n: server_n.c
	gcc -o servern server_n.c

client_n: client_n.c
	gcc -o clientn client_n.c
