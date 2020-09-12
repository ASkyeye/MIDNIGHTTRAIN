@ECHO OFF

rc Resource.rc
cvtres /MACHINE:x64 /OUT:Resource.o Resource.res
cl.exe /nologo /MT /Od /GS- /DNDEBUG /W0 /Tp Gargoyle.cpp dllmain.cpp /link Resource.o /DLL /OUT:..\\Bin\\gargoyle_x64.dll /MACHINE:x64 /RELEASE /MERGE:_RDATA=.text /EMITPOGOPHASEINFO
del Gargoyle.obj
del dllmain.obj
del Resource.o
del Resource.res