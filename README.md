# Axon v0.1.0
Axon is a lightweight and portable HTTP file transfer tool focused on making transfers between a PC and phone as easy as possible.

## Setup
- Use ````vcpkg-setup.bat````(windows) or ````vcpkg.sh````(macOS) to set up vcpkg. This will clone the vcpkg repo, bootstrap it, and install the dependencies specified in Axon's vcpkg manifest.

- Use make.bat to build the project.
  - Usage: ````make.bat (debug | release)```` 
  - Debug has a visible terminal window, while it is hidden in release. 

Axon's behavior may be configured by modifying the contents of ````settings.txt````, in the same directory as Axon. If Axon cannot find this file, it will create one with default values in the same directory as the executable.

## Usage
- ````Axon <filename>```` - starts Axon in Serve mode.
  - ````filename```` may be a relative path from the working directory, or a fully qualified filepath. 
  - Generally on Windows, dragging and dropping file X onto executable B.exe will result in the following call: ````B.exe <fully qualified path of X>````. Thus, dragging and dropping files onto Axon.exe (or a shortcut to it), will cause Axon to launch in serve mode with the dropped file.

- Pressing space in the Axon GUI will toggle between Serve and Receive mode.

### Serve mode
Axon automatically launches in receive mode. Scanning the QR code or navigating to the URL shown below it in a web browser will cause the file's contents to be sent as an HTTP attachment.

### Receive mode
Scanning the QR code will bring the user to a file upload page, hosted by the Axon server. Upon submission, the file data is transmitted back to Axon and the data is placed in a queue to be handled by the user. If there are any files in the queue, the filename of the first in the queue will appear on the right side of the Axon GUI, with a prompt to accept or discard the file. Pressing ````y```` will save the file to the directory specified in the ````save_to```` setting, relative to the executable file. Pressing ````n```` will discard the file.

## Notes - PLEASE READ
Axon is still in the alpha stages of development. It currently uses unencrypted HTTP. Users are cautioned against using Axon to transfer sensitive data.