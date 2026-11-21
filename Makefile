all: smalldis sdclient

smalldis: server.cpp
	g++ server.cpp -o smalldis
sdclient:
	g++ client.cpp -o sdclient

clean:
	rm -f smalldis sdclient
