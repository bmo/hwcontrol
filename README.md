# hwcontrol
STM32 Hardware for buttons, rotary encoder, etc


Credits

Resources Used in this endeavor:
-  The article http://www.mikrocontroller.net/articles/Drehgeber recommended by https://github.com/0xPIT
-  Pushbutton code and eventing from https://github.com/innomatica/pushbtn -- almost a complete solution!
-  A tutorial on how this stuff works:
-  Information on how one knob works: https://1vc.typepad.com/ethergeist/2011/03/origins-refinement-of-the-contest-knob-i.html
-  DDUtil was also a great source of inspiration, especially about the Z{E,R,C,L} registers
  
Additional Resources:
- https://cscott.net/usb_dev/data/devclass/usbcdc11.pdf - USB CDC Spec
- https://www.silabs.com/documents/public/application-notes/AN758.pdf - Silicon Labs app note on USB CDC Classes
- Wireshark will show packet captures taken by USBPcap
- USBPcap https://desowin.org/usbpcap - Capture USB traffic as packets, for display in Wireshark. THis was really useful to see my buffer for CDC notifications was wrong.
