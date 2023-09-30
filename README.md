# amiibo Mii Extractor
A 3DS homebrew tool to extract Miis from amiibo. I only tested this on New 3DS, I don't know if it works on Old 3DS.  
This was built off of the libctr NFC example [here](https://github.com/devkitPro/3ds-examples/tree/master/nfc).

## Installation
Simply download the 3dsx file from releases and place it in your `3ds` folder. You can then launch it from The Homebrew Launcher.

## How to Use
Upon running the app, it will begin scanning for amiibo. Place your amiibo onto the scanner (on New 3DS, it's on the bottom screen), and keep it there until prompted.  
It will then show your amiibo's Mii data on-screen. If you press A, it will save to your SD card under the name `amiibo_mii_out.bin`.

## Troubleshooting
If you encounter any errors while using this that aren't listed here, please create an issue.  
### Common Error Messages
- "Failed to start scanning. Make sure your wireless communication is enabled!"
  * Most likely your wireless communication is disabled. Use the Rosalina Menu or HOME Menu settings to enable it.
- "Failed to get amiibo data. You might have removed the amiibo too fast."
  * The app was unable to get the amiibo data. This can be caused if you removed the amiibo too fast. Be sure to keep it on the scanner until prompted to remove it.
- "This amiibo wasn't setup by the amiibo Settings applet."
  * The amiibo hasn't been properly set up yet.
- "Failed to write all data to the file. Only 0x# of 0x# bytes were written."
  * The app failed to write the Mii data to your SD card. This could be an issue with your SD card.