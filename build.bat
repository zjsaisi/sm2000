set rtesrcdir=%~dp0
::Does string have a trailing slash? if so remove it 
IF %rtesrcdir:~-1%==\ SET rtesrcdir=%rtesrcdir:~0,-1%

cd /d %rtesrcdir%

attrib -r %rtesrcdir%\ioc.hws
attrib -r %rtesrcdir%\ioc.hww
attrib -r %rtesrcdir%\IOCBoot\boot.hwp
attrib -r %rtesrcdir%\IOCHeader\iocheader.hwp
attrib -r %rtesrcdir%\IOCMain\main.hwp
attrib -r %rtesrcdir%\IOCMain\Include\version.h
attrib -r %rtesrcdir%\IOCBoot\Include\version.h
attrib -r %rtesrcdir%\IOCMain\Export\rte.dld

::IOCMain
IF NOT EXIST %rtesrcdir%\IOCMain\release (mkdir %rtesrcdir%\IOCMain\release)
del /F /Q %rtesrcdir%\IOCMain\release\*.*

cd %rtesrcdir%\IOCMain\make
c:\Hew\hmake release.mak


::IOCHeader
IF NOT EXIST %rtesrcdir%\IOCHeader\release (mkdir %rtesrcdir%\IOCHeader\release)
del /F /Q %rtesrcdir%\IOCHeader\release\*.*

cd %rtesrcdir%\IOCHeader\make
c:\Hew\hmake release.mak


::IOCBoot
IF NOT EXIST %rtesrcdir%\IOCBoot\release (mkdir %rtesrcdir%\IOCBoot\release)
del /F /Q %rtesrcdir%\IOCBoot\release\*.*

cd %rtesrcdir%\IOCBoot\make
c:\Hew\hmake release.mak


::dld
cd %rtesrcdir%
call all.bat


