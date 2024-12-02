MIT License

Copyright (c) 2024 sadam-04

# Axon

Axon is a lightweight and portable HTTP file transfer tool focused on making transfers between PCs and mobile devices as easy as possible.

## Setup

- Install [git](https://git-scm.com/downloads). Ensure git is added to the system PATH.
- Install the [Visual C++ Compiler](https://visualstudio.microsoft.com/vs/features/cplusplus/).
- Use `vcpkg-setup.bat` (windows) or `vcpkg.sh` (macOS) to set up vcpkg. This will clone the vcpkg repo, bootstrap it, and install the dependencies specified in Axon's vcpkg manifest.

- Use `make.bat` to build the project. This directly calls MSVC++'s `cl.exe`. As such, it is recommended that builds are done using the Developer Command Prompt which comes with MSVC++.
    - Usage: `make.bat (debug | release)` 
    - Debug has a visible terminal window, while it is hidden in release.

Axon's behavior may be configured by modifying the contents of `settings.txt`. See the [Settings section](#settings).

## Usage

### Launching

`Axon <filename> [/DISABLEURLSCRAMBLING]`
  - `filename` is a path to the file to be served. It may be a relative path from the working directory, or a fully qualified filepath.

  - `/DISABLEURLSCRAMBLING` replaces random alphanumeric url paths with `/download` and `/upload` for serve and receive modes, respectively.

Note: Generally on Windows, dragging and dropping file `A` onto executable `B.exe` will result in the following call being made:
````
B.exe <fully qualified path of A>
````
Thus, dragging and dropping files onto the executable (or a shortcut to it), will cause Axon to launch in serve mode with the dropped file.

### GUI

Pressing space in the Axon GUI will toggle between serve and receive mode.

**Serve mode**

Axon automatically launches in receive mode. Scanning the QR code or navigating to the URL shown below it in a web browser will cause the file's contents to be sent to the client as an HTTP attachment.

**Receive mode**

Scanning the QR code will bring the user to an upload page. Upon submission, the data is transmitted to the Axon server and placed in a queue to be handled by the server user. If there are any files in the queue, the filename of the first in the queue will appear on the right side of the Axon GUI, with a prompt to accept or discard the file.
- Pressing `y` will save the file to the directory specified in the `save_to` setting, relative to the executable file.
- Pressing `n` will discard the file.

### Settings

Axon reads setting values from `settings.txt`, in the same directory as the executable. If this file doesn't exist or cannot be opened, one will be created with [default values](#fields-and-defaults).

**User-defined variables**

Users may define their own variables to be reused across multiple fields in the settings file. The syntax is as follows. Note that identifiers must begin with `$`.

&nbsp;&nbsp;&nbsp;&nbsp;`$identifier = value`

By default, the following variables are defined in the settings file:
- `$white = FFFFFFFF`
- `$dark_blue = 222255FF`

**Comments**

Any line without the assignment operator (`=`) will be overlooked by the settings interpreter.

**Fields and defaults**

Note: Colors are encoded in hexadecimal RGBA format. E.g. Red would be encoded as `FF0000FF`.

**Host settings**

- `host` = `auto`
  - Hostname to encode in the QR and show in urls displayed in the GUI. Axon listens for requests on all adapters.
  - If this is set to `auto`, Axon will attempt to automatically determine the most useful IPv4 address at runtime.
- `port` = `8080`
  - Port for the Axon server to listen on.

**GUI color settings**

- `sendbgcolor` = `$white`
  - Background color in Send mode
- `recvbgcolor` = `$white`
  - Background color in Receive mode
- `textcolor` = `$dark_blue`
  - Text color
  
**QR color settings** - Note that extreme changes to these shades may result in an invalid or unreadable QR code.

- `qr_dark_color` = `$dark_blue`
  - Color to be used in place of black cells in the QR code
- `qr_light_color` = `$white`
  - Color to be used in place of white cells in the QR code

**Filesystem settings**

- `save_to` = `saved/`
  - Path to be prepended to files saved in receive mode, relative to the executable.

## Pre-release notice

Axon is still in the alpha stages of development. It currently uses unencrypted HTTP. Users are cautioned against using Axon to transfer sensitive data.