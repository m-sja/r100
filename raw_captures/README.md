Here is the USB-Handshake captured with Wireshark and usbmon.

To capture Data you must know the Bus on which whe Device will show up.
Just connect the Device and run "lsusb".

Open a shell
-> sudo -s
-> modproe usbmon
-> wireshark &
->

Now disconnect your USB-Device and select the corresponding Bus in Wireshark i.e. "usbmon5" (for Bus 5)
Plug in your Device and you can see the USB-Handshake.

The File r100_USB-Handshake.pcapng is my Result of the above procedure.

You will also find aditionel information like:

-> lsusb -v -d 1004:6374

-> dmesg

...
