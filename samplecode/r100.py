import usb.core
import usb.util
import sys

VID = 0x1004
PID = 0x6374

dev = usb.core.find(idVendor=VID, idProduct=PID)
if not dev:
	print "Could not find VR-Glasses :("
	exit(1)
print "r100 VR-Glasses found"

reattach = False
if dev.is_kernel_driver_active(0):
    reattach = True
    dev.detach_kernel_driver(0)

dev.set_configuration()

# Start Glasses
msg = '\x03\x0cVR App Start'
#msg = '\x03\x0cR1 Shutdown'
#msg = '\x03\x0cReboot'
dev.write(0x01, msg, 1000)

# Read response
byteread = dev.read(0x81, 32, 1000)
print byteread

exit(0)
