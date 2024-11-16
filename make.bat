@ECHO OFF
mkdir out
cl src/main.cpp src/nayuki-qr/qrcodegen.cpp src/webserver.cpp src/dataloader.cpp src/qr_to_bmp.cpp src/window.cpp /Fe:out/Axon.exe /Fo:out/ /I src/SFML-2.6.2/include /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /LIBPATH:"src\SFML-2.6.2\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib