import usb.core

dev = usb.core.find(idVendor=0x1004, idProduct=0x6374)
if dev is None:
    raise ValueError('Our device is not connected')
else:
    print('LG-VR Glasses connected')


#Check if USB-Device of specific Class is connected
# http://www.usb.org/developers/defined_class
if usb.core.find(bDeviceClass=9) is None:
    raise ValueError('No HUB found')
else:
    print('USB-HUB conneced')


# list all USB-HUBs
printers = usb.core.find(find_all=True, bDeviceClass=9)
import sys
sys.stdout.write('There are ' + len(printers) + ' in the system\n.')
