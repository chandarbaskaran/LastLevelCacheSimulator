
all:
	gcc -o cacheapp.exe BusOp.cpp CacheOp.cpp LRU.cpp TraceDecoder.cpp
clean: 
	rm cacheapp.exe
