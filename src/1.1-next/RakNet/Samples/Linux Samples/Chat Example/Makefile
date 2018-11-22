INCLUDE_DIR=../../../Include/

all: chat_client chat_server

chat_client:
	g++ -I$(INCLUDE_DIR) -lraknet chat_client.cpp -o client

chat_server:
	g++ -I$(INCLUDE_DIR) -lraknet chat_server.cpp -o server

clean:
	rm -f client
	rm -f server
	rm -f *.o
