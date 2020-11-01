game.exe: game.c dialog.res
	cl /I "C:\Projects\BICDev\pthreads-win32" /I "C:\Program Files\Microsoft SDKs\Windows\v7.1\Include" /I "C:\Projects\BICDev\dyad\include" game.c dialog.res /EHsc /link /LIBPATH:"C:\Projects\BICDev\pthreads-win32" /LIBPATH:"C:\Program Files\Microsoft SDKs\Windows\v7.1\Lib" /LIBPATH:"C:\Projects\BICDev\dyad\lib" user32.lib gdi32.lib pthreadVC2.lib ws2_32.lib dyad.lib

dialog.res: dialog.rc resource.h
	rc /i "c:\Program Files\Microsoft SDKs\Windows\v7.1\Include" /fo dialog.res dialog.rc 

.PHONY: clean

clean:
	del dialog.res game.obj game.exe
