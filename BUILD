Build instructions for Qt 5.5

##  macOS 

Prerequisites:

    brew install qt

Build:

    $(brew --prefix qt)/bin/qmake
    make


## Windows

Dynamically linked executable:

1. Build it
2. Copy `towcam.exe` to a new directory
3. Run c:\Qt\5.12.3\mingw73_32\bin\windeployqt.exe towcam.exe
   This copies MOST of the dependencies into the directory
4. Copy from c:\Qt\5.12.3\mingw73_32\bin:
    libgcc_s_dw2-1.dll
    libstdc++-6.dll
    libwinpthread-1.dll

TODO: statically linked executable
