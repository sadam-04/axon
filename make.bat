@ECHO OFF
mkdir out
if "%1"=="debug" (
    echo Compiling debug version...
    mkdir "out/debug/"
    cl src/main.cpp src/nayuki-qr/qrcodegen.cpp src/webserver.cpp src/dataloader.cpp src/qr_to_bmp.cpp src/window.cpp /Fe:out/debug/Axon.exe /Fo:out/debug/ /I src/SFML-2.6.2/include /link /LIBPATH:"src\SFML-2.6.2\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib
) else (
    echo Compiling prod version...
    mkdir "out/prod/"
    cl src/main.cpp src/nayuki-qr/qrcodegen.cpp src/webserver.cpp src/dataloader.cpp src/qr_to_bmp.cpp src/window.cpp /Fe:out/prod/Axon.exe /Fo:out/prod/ /I src/SFML-2.6.2/include /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /LIBPATH:"src\SFML-2.6.2\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib
)