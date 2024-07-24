# WHOI TowCam Data Logging GUI
A simple logging GUI for MISO TowCam: https://www2.whoi.edu/site/miso/miso-instrumentation/towcam/
The GUI displays live feeds of TowCam altimeter, depth, and forward-looking altimeter (if installed)
for use during operations.

- Source repository: https://github.com/WHOIGit/TowCam/

# Building

The TowCam GUI requires Qt6 (https://www.qt.io/download-open-source) and CMake (https://cmake.org/).
Obtaining Qt and CMake varies by OS.  Additionally, for XCode (https://developer.apple.com/xcode/)
is required for building TowCam for Apple OSX.

To build TowCam:

```
git clone https://github.com/WHOIGit/TowCam   # Clone this git repository
cd towcam
mkdir build && cd build                       # Make a build directory
cmake .. && cmake --build .                   # Configure and build the 'towcam' application.
cpack -G $GEN                                 # Package the application.  Typical values of $GEN would be:
                                              # Windows: ZIP  (ZIP archive w/ Qt libraries)
                                              # Linux:   TGZ  (gzipped tar archive (.tar.gz) w/ Qt libraries)
                                              # OSX:     DragNDrop (Disk image (.dmg) with shortcut to Applications for install)
```

# Configuration
The GUI looks for a file named `towcam.ini` in the `Desktop` folder.  An example configuration file
is provided in this repository.

# Logging
Data is logged to a folder named `towcamData` that will be created in the `Desktop` folder.
