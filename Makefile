EXEC= client server

normal: $(EXEC)

client: client.c
	gcc -pthread client.c -o client 

server: server.c
	gcc -pthread server.c -o server 

clean:
	rm $(EXEC)