# Axon
Axon is a lightweight and portable file transfer tool focused on making transfers between a PC and phone as easy as possible.

## Setup
- Use ````vcpkg-setup.bat````(windows) or ````vcpkg.sh````(macOS) to set up vcpkg. Specifically, this will clone the vcpkg repo, bootstrap, and install the dependencies specified in Axon's vcpkg manifest.

- The Axon repo provides make.bat, which can be used to build the project.
  - ````make.bat (debug | release)```` 
  - Debug has a visible terminal window, while it is invisible in release. 

Axon's behavior may be configured by modifying the contents of ````settings.txt````. If Axon cannot find this file, it will create one with default values in the same directory as ````Axon.exe````.

## Usage
- ````Axon <filename>```` - starts Axon in Serve mode.
  - ````filename```` may be a relative path from the working directory, or a fully qualified filepath. 
  - Generally on Windows, dragging and dropping file X onto executable B.exe will result in the following call: ````B.exe <fully qualified path of X>````. Thus, dragging and dropping files onto Axon.exe (or a shortcut to it), will work fine.

- Pressing space in the Axon GUI will toggle between Serve and Receive modes.

## Notes
Axon is still in the alpha stages of development. Be aware that it currently uses unencrypted HTTP. Users are encouraged to avoid using Axon to transfer sensitive data.