CC=cl
INCDIR=/I "C:\Projects\BICDev\pthreads-win32" /I "C:\Program Files\Microsoft SDKs\Windows\v7.1\Include" /I "C:\Projects\BICDev\dyad\include"
LIBDIR=/LIBPATH:"C:\Projects\BICDev\pthreads-win32" /LIBPATH:"C:\Program Files\Microsoft SDKs\Windows\v7.1\Lib" /LIBPATH:"C:\Projects\BICDev\dyad\lib"
LIBS=pthreadVC2.lib ws2_32.lib dyad.lib

all: server.exe client.exe

server.exe: server.c
	$(CC) $(INCDIR) server.c /link $(LIBDIR) $(LIBS)

client.exe: client.c
	$(CC) $(INCDIR) client.c /link $(LIBDIR) $(LIBS) 

.PHONY: clean

clean:
	del *.obj *.exe

