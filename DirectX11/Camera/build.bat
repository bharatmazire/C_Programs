cls

del D3D11.exe

cl.exe /c /EHsc D3D11.cpp

link.exe D3D11.obj user32.lib gdi32.lib kernel32.lib /SUBSYSTEM:WINDOWS /MACHINE:X64

del D3D11.obj

D3D11.exe
