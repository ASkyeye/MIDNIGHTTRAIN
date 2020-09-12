@ECHO OFF

cl.exe /nologo /MT /Od /GS- /DNDEBUG /W0 /Tp Gremlin.cpp dllmain.cpp portmon.def /link /DLL /OUT:..\\Bin\\gremlin_x64.dll /MACHINE:x64 /RELEASE /MERGE:_RDATA=.text /EMITPOGOPHASEINFO
del Gremlin.obj
del dllmain.obj
del ..\\Bin\\*.lib
del ..\\Bin\\*.exp