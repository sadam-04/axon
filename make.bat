@ECHO OFF
setlocal
set PROJECT_ROOT=%~dp0
set VCPKG_ROOT=%PROJECT_ROOT%vcpkg_installed
set TRIPLET=x64-windows
mkdir out
if "%1"=="debug" (
    echo Compiling debug version...
    mkdir "out\debug\"
    cl /EHsc src\main.cpp src\nayuki-qr\qrcodegen.cpp src\webserver.cpp src\dataloader.cpp src\qr_to_bmp.cpp src\window.cpp src\gethost.cpp src\randompath.cpp src/settings/settings.cpp src\icon\icon.res /Fe:out\debug\Axon.exe /Fo:out\debug\ /I "%VCPKG_ROOT%\%TRIPLET%\include" /link /LIBPATH:"%VCPKG_ROOT%\%TRIPLET%\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib
    xcopy src\static out\debug\static /E /I /H /Y
    xcopy %VCPKG_ROOT%\%TRIPLET%\bin\* out\debug\* /Y
) else (
    echo Compiling prod version...
    mkdir "out/prod/"
    ::Same as above, but add /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup 
    cl /EHsc src\main.cpp src\nayuki-qr\qrcodegen.cpp src\webserver.cpp src\dataloader.cpp src\qr_to_bmp.cpp src\window.cpp src\gethost.cpp src\randompath.cpp src/settings/settings.cpp src\icon\icon.res /Fe:out\debug\Axon.exe /Fo:out\debug\ /I "%VCPKG_ROOT%\%TRIPLET%\include" /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /LIBPATH:"%VCPKG_ROOT%\%TRIPLET%\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib
    xcopy src\static out\debug\static /E /I /H /Y
    xcopy %VCPKG_ROOT%\%TRIPLET%\bin\* out\debug\* /Y
)
endlocal