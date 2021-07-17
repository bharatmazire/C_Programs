cls

cl.exe /c /EHsc /I"C:\glew\include" OGLPP.cpp

del OGLPP.exe

link.exe /LIBPATH:"C:\glew\lib\Release\x64" OGLPP.obj user32.lib gdi32.lib kernel32.lib opengl32.lib 

del OGLPP.obj

OGLPP.exe
