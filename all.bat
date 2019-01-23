

@set CURR_DIR=IOCBoot\release
copy IOCHeader\release\IOCHeader.mot + %CURR_DIR%\boot.mot tmp.mot
conv -DLD -i tmp.mot -o boot.dld

@rem @set CURR_DIR=iocfpga\release
@rem copy IOCHeader\release\IOCHeader.mot + %CURR_DIR%\FPGA.mot tmp.mot
@rem conv -DLD -i tmp.mot -o fpga.dld

@set CURR_DIR=IOCMain\debug
copy  IOCHeader\release\IOCHeader.mot + %CURR_DIR%\main.mot tmp.mot
conv -DLD -i tmp.mot -o maindebug.dld

@set CURR_DIR=IOCMain\release
copy IOCHeader\release\IOCHeader.mot + %CURR_DIR%\main.mot tmp.mot
conv -DLD -i tmp.mot -o rte.dld

@del tmp.mot
@date /t
@time /t
