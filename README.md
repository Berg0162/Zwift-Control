# Zwift-Control
Simple Zwift BLE Remote Control with only 2 buttons

## Versions for 2 different SOC's:
- ESP32 with H2Zero's NimBLE library
- nRF52840 with Adafruit's Bluefruit library

## What it does:
The code turns an ESP32/nRF52 board into a 2 button (with 3 states) Bluetooth LE controller. Optimized for use with Zwift game to remotely initiate specific game actions that are mapped (by the Zwift app) to different keys of the regular keyboard.<br> 
In short: mimics some keys of your keyboard

## Low cost DIY alternative for:
- Keith Wakeham's [Kommander](https://titanlab.co/715-2/) <br>
Review by DCRainmain: [Zwift Control For Your Handlebars](https://www.dcrainmaker.com/2021/02/kommander-review-zwift-control-for-your-handlebars.html)
- Some functionality of Zwift Play Controller (<b>NO</b> steering!)

## Mapping of 2 buttons to game actions
The code has implemented a limited set of the official mapping of keyboard shortcuts during the Zwift game: [see keyboard shortcuts](https://support.zwift.com/en_us/keyboard-shortcuts-rkGrgwd4B)<br>
Default Zwift keyboard mapping is one key to one action! The present code allows for 3 actions mapped to one key, since it can detect the difference between a single button click, a double button click and a "long hold" button click (longer than 0.5 second). Each of these will initiate a different game action.<br>
In addition the Zwift game knows two different modes in the game: Freeride/Race and Workout mode. Some of the mappings will have only effect if in the "right" mode. The code will handle the different modes without your intervention, like is detailed hereafter:
### Freeride/Race
|Action|Button #1|Button #2|
|---------------|--------------------------|--------------------------|
|One Click|Turn left at intersection|Turn right at intersection|
|Double Click|Power Up|Make an U-turn|
|Long|Change your Camera View| |

### Workout mode
|Action|Button #1|Button #2|
|---------------|--------------------------|--------------------------|
|One Click|Increase FTP|Decrease FTP|
|Double Click| | |
|Long|Change your Camera View|Skip Workout Step|
