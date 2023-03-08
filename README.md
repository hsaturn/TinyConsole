# NOTE : Unit tests fails, but this lib is ok
The failed test does not impact the library. The TinyString class is not yet used (the one that fails).

# TinyConsole
This library allows to communicate with an Arduino or an ESP through terminal  applications such as Minicomm or Mobaxterm.
It supports a subset of ansi escape sequence and is able to distinguish between the Serial Monitor of Arduino IDE and a ansi terminal.

A prompt allows to enter commands that can be interpreted trhough a user defined callback.

[![Release](https://img.shields.io/github/v/release/hsaturn/TinyConsole)](https://github.com/hsaturn/TinyConsole/releases)
[![AUnit Tests](https://github.com/hsaturn/TinyConsole/actions/workflows/aunit.yml/badge.svg)](https://github.com/hsaturn/TinyConsole/actions/workflows/aunit.yml)
[![Issues](https://img.shields.io/github/issues/hsaturn/TinyConsole)](https://github.com/hsaturn/TinyConsole/issues)
[![Esp8266](https://img.shields.io/badge/platform-ESP8266-green)](https://www.espressif.com/en/products/socs/esp8266)
[![Esp32](https://img.shields.io/badge/platform-ESP32-green)](https://www.espressif.com/en/products/socs/esp32)
[![Gpl 3.0](https://img.shields.io/github/license/hsaturn/TinyConsole)](https://www.gnu.org/licenses/gpl-3.0.fr.html)

## Screenshot
![Screenshot](/TinyConsole.png)

## Features

- Detection of ansi term vs Arduino's IDE serial monitor
- Command line with prompt
- Home, Delete, Backspace, and Cursors keys are handled.
- Separate callback when the user hit a function key (F1..F12)
- Supports Ansi colors
- Subset of ansi codes (cursor position, clear, reset, cursor visibility)
- TinyString class designed to replace std::string, and more.

## Limitations

- Line per line evaluation (no key event)
- Not all ansi sequences are implemented
- Size of terminal not detected (yet)

## Examples

| Example             | Description                                |
| ------------------- | ------------------------------------------ |
| [console](https://github.com/hsaturn/TinyConsole/tree/main/examples/console/console.ino) | Minimal example showing callbacks usages |

## TODO List
* Detect terminal size
* Separate command line from terminal
* Send event for each keys

## License
Gnu GPL 3.0, see [LICENSE](https://github.com/hsaturn/TinyMqtt/blob/main/LICENSE).

