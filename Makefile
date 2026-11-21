all: smalldis sdclient

smalldis: server.cpp
	g++ -Wall -Wextra -O2 -g server.cpp -o smalldis
sdclient: client.cpp
	g++ -Wall -Wextra -O2 -g client.cpp -o sdclient

clean:
	rm -f smalldis sdclient
