FF4Tools [![Codacy Badge](https://app.codacy.com/project/badge/Grade/4bb5f73ba00d42fea3fc06470245ff61)](https://www.codacy.com/gh/marco-calautti/FF4Tools/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=marco-calautti/FF4Tools&amp;utm_campaign=Badge_Grade)
========

Toolset written in C++ to manage archives and compressed files from the game Final Fantasy 4: The Complete Collection.
If you are also interested in editing graphics files from this game, you should definitely checkout [Rainbow](https://github.com/marco-calautti/Rainbow).

## Building on Windows
Make sure that boost is installed in C:\boost_library and it has been compiled with Visual Studio 2013 (msvc12) in multi-threaded mode (mt).

Open the solution file in Visual Studio 2013 Express for Desktop.

Select Release as target and press F7. Binaries can be found in the Release directory.

## Building on Linux
The boost library needs to be installed.
eg. on Arch Linux:
`pacman -S boost`

Open a terminal on the root directory (where the Makefile is located) and type:

`make`

Executables should be found in the same directory. Make them executable with chmod +x or automatically install them
to /usr/bin by typing:

`sudo make install`

To uninstall everything:

`sudo make uninstall`

To clean:

`sudo make clean`
