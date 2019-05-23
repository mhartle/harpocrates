all: crcwrap crcunwrap

crcwrap:
	g++ -o crcwrap -I include crcwrap.cpp
	strip crcwrap

crcunwrap:
	g++ -o crcunwrap -I include crcunwrap.cpp
	strip crcunwrap

clean:
	rm -f crcwrap
	rm -f crcunwrap
