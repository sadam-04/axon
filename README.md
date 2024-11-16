# Axon
Quick and lightweight file transfer tool.

# Setup
Install SFML 2.6.2 to src/SFML-2.6.2</br>
Directory structure should look like this:</br>
src/</br>
    SFML-2.6.2/</br>
        bin/</br>
        doc/</br>
        examples/</br>
        include/</br>
        lib/</br>
        ...</br>
Then, copy the following to the executable directory:</br>
 - sfml-graphics-2.dll
 - sfml-system-2.dll
 - sfml-window-2.dll

 Finally, create a file called host.txt also in the executable directory. The first line should contain the host ip address to encode in the QR code. The second line should be the port. Example:
 ```
192.168.1.20
8080
 ```
