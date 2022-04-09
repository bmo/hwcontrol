# hwcontrol - A tuning knob for Flex Radio, RHR, SmartSDR
Now you can build your own knob (BYOK!)

This is a STM32-based project for a tuning knob with pushbuttons and lights. 

![build-your-own-knob](https://user-images.githubusercontent.com/11912/162589593-0076b550-c16c-495f-9773-ae2df6a92376.jpg)

Compatible with PowerSDR, SmartSDR, and Remote Ham Radio (use a knob with RHR!)

see https://github.com/bmo/tuning-knob-pcboard for the KiCAD files to make PC Boards for this project.

## Credits

### Resources Used in this endeavor:
-  The article http://www.mikrocontroller.net/articles/Drehgeber recommended by https://github.com/0xPIT
-  Pushbutton code and eventing from https://github.com/innomatica/pushbtn -- almost a complete solution!
-  A tutorial on how this stuff works:
-  Information on how one knob works: https://1vc.typepad.com/ethergeist/2011/03/origins-refinement-of-the-contest-knob-i.html
-  DDUtil was also a great source of inspiration, especially about the Z{E,R,C,L} registers
  
### Additional Resources:
- https://cscott.net/usb_dev/data/devclass/usbcdc11.pdf - USB CDC Spec
- https://www.silabs.com/documents/public/application-notes/AN758.pdf - Silicon Labs app note on USB CDC Classes
- Wireshark will show packet captures taken by USBPcap
- USBPcap https://desowin.org/usbpcap - Capture USB traffic as packets, for display in Wireshark. This was really useful to see my buffer for CDC notifications was wrong.

### Futures
- Present as a USB composite device with an additional serial port for a debounced footswitch input (all in one for RHR)
- RGBW LED control for cool effects
- Update via UF2
- Support for keyboard-type keyswitches
- Support for alternative processors and hardware
