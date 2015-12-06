# Delovi Logfile Viewer

Description yet to be written.



# How to build



## How to build on Linux

Howto is yet to be written.



## How to build on Microsoft Windows

* Only Visual Studio 2013 and Visual Studio 2015 and only 32 bit builds are supported.
* You need Qt 4.8 (at least 4.8.5), Boost 1.56, and CMake 2.8 or 3.3.
* Qt:
  * Build Qt:
    * Since Qt 4.8.6 won't build on Windows using Visual Studio 2015 out of the box there are some modifications to be made in order to build Qt. See http://stackoverflow.com/questions/32848962/how-to-build-qt-4-8-6-with-visual-studio-2015-without-official-support for how to do that.
    * Then use these steps to build Qt:
      * Open the VS2015 x86 Native Tools Command Prompt and run:
      ```
      >set QTDIR=C:\Qt\4.8.6
      >cd /D %QTDIR%
      >configure.exe -debug-and-release -make nmake -opensource -ltcg -qt-sql-sqlite -no-qt3support -platform win32-msvc2015 -qt-zlib -qt-libpng -qt-libmng -qt-libtiff -qt-libjpeg -no-dsp -no-vcproj -mmx -3dnow -sse -sse2 -no-openssl -no-dbus -no-phonon -no-webkit -script -scripttools -no-declarative -arch windows -qt-style-windowsxp -qt-style-windowsvista -mp
      ```
  * For building Delovi some environment variables need to be set (replace ? with appropriate number):
    * `QTDIR` must contain the path to the Qt directory, e.g. `C:\Qt\4.8.?`
    * `QMAKESPEC` must contain: `win32-msvc201?`
* Boost:
  * Build Boost:
    * Use these steps to build Boost (replace ? with appropriate number):
      * Open the VS201? x86 Native Tools Command Prompt and run:
      ```
      >set BOOSTDIR=C:\Boost\1.5?.0
      >cd /D %BOOSTDIR%
      >bootstrap.bat
      >b2 --prefix=_prefix --exec-prefix=_eprefix --libdir=lib32-msvc201? --stagedir=_stagedir --build-type=complete --build-dir=_builddir
      ```
  * For building Delovi some environment variables need to be set:
    * `BOOSTDIR` must contain the path to the Boost directory, e.g. `C:\Boost\1.56.0`
    * Because the CMake configuration of this solution requires the Boost libraries to be found in `%BOOSTDIR%\lib_%Platform%`, the variable `Platform` must contain `X86` for 32 bit builds and `X64` for 64 bit builds.
* Since CMake is required to build the solution the environment variable `PATH` needs to contain the path to the CMake executable.
* For patching the generated `CMakeCache.txt` you need the patch tool:
  * Because the CMake generated file `CMakeCache.txt` doesn't contain all the recommended compile and link flags there's a patch file `CMakeCache.patch` that can be applied to the fresly generated `CMakeCache.txt`.
  * For this to work the patch tool (known from the Linux world) is required.
  * For Windows there's a port that can be downloaded from: http://gnuwin32.sourceforge.net/packages/patch.htm
  * However, Windows Vista, 7, 8, 8.1, and later thinks the executable `patch.exe` requires administrator rights, which is very annoying.
  * The simple solution is to rename the executable to e.g. `pat_ch.exe`. See also: http://superuser.com/questions/894170/patch-tool-without-admin-privileges-on-windows-7
  * Naturally it is most helpful if your environment variable `PATH` contains the path to the patch executable.



### How to build using Microsoft Visual Studio 2013

* Open the VS2013 x86 Native Tools Command Prompt.
* Set some environment variables:
```
>set Platform=X86
>set BOOSTDIR=C:\Boost\1.56.0
>set QTDIR=C:\Qt\4.8.5
>set QMAKESPEC=win32-msvc2013
>set CMAKEDIR=C:\Tools\cmake-2.8\bin
>set Path=%Path%;%QTDIR%\bin;%CMAKEDIR%
```
* Go to your git checkout directory and there change directory to subfolder `BUILD_MSVC2013`.
* Start `cmake-gui`. There you will be asked for the location of the source code and the build directory. The source code path is the path of the git checkout. The build directory is the subdirectory of that path: `<CHECKOUTPATH>/BUILD_MSVC2013`.
* Press the button "Configure". In the upcoming dialog select the compiler you're using (with MSVC 2013 this is usually "Visual Studio 12") and select "Use default native compilers". Press the button "Finish" and wait until the process is finished.
* Press the button "Generate" and wait until the process is finished.
* Close `cmake-gui`.
* Use the patch tool to modify some CMake-settings in `CMakeCache.txt`:
```
>pat_ch CMakeCache.txt CMakeCache.patch
```
* Start `cmake-gui` again.
* Press the button "Generate" once more. Wait until the process finished.
* Close `cmake-gui`.
* If you want to change the path where the built executables will be installed:
  * Open `CMakeCache.txt` in an editor.
  * Search for `CMAKE_INSTALL_PREFIX`.
  * Change that path to your liking, e.g.: `C:/Tools/DELOVI`.
  * Re-run `cmake-gui` again and press "Generate" button.
* Start Visual Studio 2013 and open the generated solution you've just created:
```
>devenv DELOVI.sln
```
* Or in case you just want to build and install the application run:
```
>devenv DELOVI.sln /Build "Release|Win32" /Project INSTALL
```



### How to build using Microsoft Visual Studio 2015

* Open the VS2015 x86 Native Tools Command Prompt.
* Set some environment variables:
```
>set Platform=X86
>set BOOSTDIR=C:\Boost\1.59.0
>set QTDIR=C:\Qt\4.8.6
>set QMAKESPEC=win32-msvc2015
>set CMAKEDIR=C:\Tools\cmake-3.3\bin
>set Path=%Path%;%QTDIR%\bin;%CMAKEDIR%
```
* Go to your git checkout directory and there change directory to subfolder `BUILD_MSVC2015`.
* Start `cmake-gui`. There you will be asked for the location of the source code and the build directory. The source code path is the path of the git checkout. The build directory is the subdirectory of that path: `<CHECKOUTPATH>/BUILD_MSVC2015`.
* Press the button "Configure". In the upcoming dialog select the compiler you're using (with MSVC 2015 this is usually "Visual Studio 14 2015") and select "Use default native compilers". Press the button "Finish" and wait until the process is finished.
* Press the button "Generate" and wait until the process is finished.
* Close `cmake-gui`.
* Use the patch tool to modify some CMake-settings in `CMakeCache.txt`:
```
>pat_ch CMakeCache.txt CMakeCache.patch
```
* Start `cmake-gui` again.
* Press the button "Generate" once more. Wait until the process finished.
* Close `cmake-gui`.
* If you want to change the path where the built executables will be installed:
  * Open `CMakeCache.txt` in an editor.
  * Search for `CMAKE_INSTALL_PREFIX`.
  * Change that path to your liking, e.g.: `C:/Tools/DELOVI`.
  * Re-run `cmake-gui` again and press "Generate" button.
* Start Visual Studio 2015 and open the generated solution you've just created:
```
>devenv DELOVI.sln
```
* Or in case you just want to build and install the application run:
```
>devenv DELOVI.sln /Build "Release|Win32" /Project INSTALL
```
