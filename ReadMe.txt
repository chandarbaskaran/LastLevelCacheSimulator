Course ECE 585 (Winter 2017)
Project: Last-level Cache Simulator

Build Commands:
To build the executable using gcc from Cygwin shell, navigate to
this path and use the following
commands are available:
1) Building cacheapp.exe:
	make all
2) Cleaning cacheapp.exe:
	make clean

To build the project using Visual Studio on Windows, use Visual Studio 2015.

Run Commands:
1) From Cygwin:
	./cacheapp.exe -f <tracefilename>
	E.g.: ./cacheapp.exe -f cc1.din
2) From Windows Command Prompt:
	cacheapp.exe -f <tracefilename>
	E.g.: cacheapp.exe -f cc1.din
