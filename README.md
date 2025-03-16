# clipman2

`clipman2` is a basic clipboard manager for Wayland, designed to replace the `clipman` project. The goal of this project is to resolve the issues inherent in the `clipman` project.

The purpose of this program is to preserve clipboard content when the applications from which the content was copied are closed.

## Features
- Resolves issues with the first copy operation sometimes failing, requiring a second attempt.
- Eliminates disk usage by storing clipboard data in RAM (shared memory), reducing wear on SD cards or SSDs.
- Fixes problems with disappearing clipboard content.
- Works seamlessly with Wayland-based compositors.


## Installation

Ensure that the `wl-clipboard` package is installed as a prerequisite.

### Clone the repository
```bash
git clone https://github.com/qrp73/clipman2.git
cd clipman2
```

### Build and Install
```bash
mkdir build && cd build
cmake ..
make
sudo make install
```


## Usage

You can verify the functionality of `clipman2` by running the following command in the terminal:

```bash
/usr/bin/wl-paste -t "text/plain;charset=utf-8" --watch /usr/bin/clipman2
```
While this command is running, any text copied from an application that you close will remain in the clipboard, allowing you to continue pasting it without issues. 

If everything works as expected, you can proceed to add `clipman2` to your compositor's configuration.

### Wayland Wayfire

To use `clipman2` with Wayfire, add the following line to your `~/.config/wayfire.ini`:
```
clipman-store = wl-paste -t "text/plain;charset=utf-8" --watch /usr/bin/clipman2
```

### Wayland Labwc

To use `clipman2` with Labwc, add the following line to your `~/.config/labwc/autostart`:
```
/usr/bin/wl-paste -t "text/plain;charset=utf-8" --watch /usr/bin/clipman2 &
```


## Uninstall

To uninstall, remove the reference to `clipman2` from `~/.config/wayfire.ini` or `~/.config/labwc/autostart`, then execute the following command:

```bash
sudo rm /usr/bin/clipman2
```


## License

This project is licensed under the GNU General Public License v3.0, see the LICENSE file for details.
