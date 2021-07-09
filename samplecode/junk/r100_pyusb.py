import usb.core
import usb.util

# find our device
dev = usb.core.find(idVendor=0x1004, idProduct=0x6374)

# was it found?
if dev is None:
    raise ValueError('Device not found')

# actually this is not the whole history, keep reading
if usb.core.find(bDeviceClass=0) is None:
    raise ValueError('No hid found')

# set the active configuration. With no arguments, the first
# configuration will be the active one
dev.set_configuration(0)

# get an endpoint instance
cfg = dev.get_active_configuration()
intf = cfg[(0,0)]

ep = usb.util.find_descriptor(
    intf,
    # match the first OUT endpoint
    custom_match = \
    lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_OUT)

assert ep is not None

# write the data
ep.write('VR App Start')
