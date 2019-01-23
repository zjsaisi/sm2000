-------- PROJECT GENERATOR --------
PROJECT NAME :	Boot
PROJECT DIRECTORY :	C:\Miao\IOC\Boot
CPU SERIES :	2600
CPU TYPE :	2633F
TOOLCHAIN NAME :	Hitachi H8S,H8/300 Standard Toolchain
TOOLCHAIN VERSION :	3.0A.0
GENERATION FILES :
    C:\Miao\IOC\Boot\dbsct.c
        Setting of B,R Section
    C:\Miao\IOC\Boot\sbrk.c
        Program of sbrk
    C:\Miao\IOC\Boot\iodefine.h
        Definition of I/O Register
    C:\Miao\IOC\Boot\intprg.cpp
        Interrupt Program
    C:\Miao\IOC\Boot\vecttbl.cpp
        Initialize of Vector Table
    C:\Miao\IOC\Boot\vect.h
        Definition of Vector
    C:\Miao\IOC\Boot\resetprg.cpp
        Reset Program
    C:\Miao\IOC\Boot\hwsetup.cpp
        Hardware Setup file
    C:\Miao\IOC\Boot\Boot.cpp
        Main Program
    C:\Miao\IOC\Boot\sbrk.h
        Header file of sbrk file
    C:\Miao\IOC\Boot\stacksct.h
        Setting of Stack area
LIBRARY NAME :	c8s26a.lib, ec226a.lib
START ADDRESS OF SECTION :
    H'000000000	DVECTTBL,DINTTBL
    H'000000400	PResetPRG,PIntPRG
    H'000000800	P,C,D,C$DSEC,C$BSEC
    H'000FFB000	B,R
    H'000FFEDB0	S

* When the user program is executed,
* the interrupt mask has been masked.
* 
* ****H8S/2633F Advanced****
