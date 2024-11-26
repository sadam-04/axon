@ECHO OFF
setlocal
set PROJECT_ROOT=%~dp0
set VCPKG_ROOT=%PROJECT_ROOT%vcpkg_installed
set TRIPLET=x64-windows
mkdir out
if "%1"=="debug" (
    echo Compiling debug version...
    mkdir "out\debug\"
    cl /EHsc /std:c++17 src\*.cpp src\common\*.cpp src\settings\*.cpp src\nayuki-qr\qrcodegen.cpp src\icon\icon.res /Fe:out\debug\Axon.exe /Fo:out\debug\ /I "%VCPKG_ROOT%\%TRIPLET%\include" /link /LIBPATH:"%VCPKG_ROOT%\%TRIPLET%\lib"
    xcopy src\static out\debug\static /E /I /H /Y >nul
    for %%I in (
        sfml-graphics-2.dll
        sfml-window-2.dll
        sfml-system-2.dll
        freetype.dll
        libpng16.dll
        bz2.dll
        brotlidec.dll
        brotlicommon.dll
    ) do copy %VCPKG_ROOT%\%TRIPLET%\bin\%%I out\debug\ >nul
)
if "%1"=="release" (
    echo Compiling release version...
    mkdir "out\release\"
    cl /EHsc /std:c++17 src\*.cpp src\common\*.cpp src\settings\*.cpp src\nayuki-qr\qrcodegen.cpp src\icon\icon.res /Fe:out\release\Axon.exe /Fo:out\release\ /I "%VCPKG_ROOT%\%TRIPLET%\include" /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /LIBPATH:"%VCPKG_ROOT%\%TRIPLET%\lib"
    xcopy src\static out\release\static /E /I /H /Y >nul
    for %%I in (
        sfml-graphics-2.dll
        sfml-window-2.dll
        sfml-system-2.dll
        freetype.dll
        libpng16.dll
        bz2.dll
        brotlidec.dll
        brotlicommon.dll
    ) do copy %VCPKG_ROOT%\%TRIPLET%\bin\%%I out\release\ >nul
)