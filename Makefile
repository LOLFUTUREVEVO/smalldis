all: smalldis

smalldis:
	g++ server.cpp -o smalldis

clean:
	rm -f smalldis
