/**
 * A minimal version to check Canoscan Lide 210
 * button presses (e.g. autoscan button).
 *
 * License: GPL
 *
 * You need to have libusb-dev installed.
 * apt-get install libusb-dev
 *
 * Compile:
 * g++ -Wall canoscan_button.cpp -o canoscan_button -lusb
 *
 * Run as normal user:
 * ./canoscan_button
 *
 * Debug with:
 * wireshark
 *  => choose the correct USB bus
 */

#include <iostream>

extern "C" {
 #include <usb.h>
 #include <stdio.h>
}

using namespace std;

// Check with lsusb in command line
// Bus 00x Device 00y: ID 1004:6374 LG Electronics, Inc.
#define VENDOR_ID  0x1004
#define PRODUCT_ID 0x6374


// get a handle to the USB device and configure it
usb_dev_handle*
get_handle(struct usb_device *usbDevice)
{
  usb_dev_handle *deviceHandle = NULL;
  if( usbDevice == NULL ) {
    return NULL;
  }

  deviceHandle = usb_open( usbDevice );

  if (deviceHandle == NULL)
  {
    cerr << "Unable to open USB device "
         << usb_strerror() << endl;
    usb_close(deviceHandle);
    exit(-1);
  }
  return deviceHandle;
}

// find the right USB device
struct usb_device*
findUSBDevice(void) {
  struct usb_bus *bus;
  struct usb_device *usbDevice;
  struct usb_device *r100USBDevice = NULL;

  usb_init();
  usb_find_busses();
  usb_find_devices();

  // first search for r100 device
  for (bus = usb_busses; bus; bus = bus->next) {
    for (usbDevice = bus->devices; usbDevice;
         usbDevice = usbDevice->next)
    {
      if (usbDevice->descriptor.idVendor == VENDOR_ID &&
          usbDevice->descriptor.idProduct == PRODUCT_ID)
      {
        r100USBDevice = usbDevice;
        break;
      }
    }
  }

  if (r100USBDevice == NULL) {
    cerr << "No LG VR-Glasses found." << endl;
    ::exit(-1);
  } else {
    cout << "LG VR-Glasses r100 found as device: "
         << r100USBDevice->filename << "." << endl;
  }

  return r100USBDevice;
}

/**
 * Possible Commands
 * getDebugInfo
 * getAATResult
 * Accel On
 * Accel Off
 * Accel Cal
 * Accel Selftest
 * Accel Get XYZ
 * Gyro On
 * Gyro Off
 * Gyro Cal
 * Gyro Selftest
 * Gyro Get XYZ
 * Compass On
 * Compass Off
 * Compass Get XYZ
 * Proximity On
 * Proximity Off
 * Proximity Cal
 * Proximity Get Data
 * Proximity Get Crosstalk'
 * Proximity Set Regi
 * Proximity Get Regi
 * isDisplayReady
 * Reboot
 * Set Brightness
 * Set Mode
 * Set Backlight Control
 * Set LCD Pattern Test
 * R1 Shutdown
 * VR App Start
 * Sleep Disable
 * Read MiniOS Result
 * Write Motion Result
 * Write Proximity Result
 * Write Key Result
 * Write Earphone Result
 * Write HDMI Result
 * Write LCD Result
 * Write Reset Result
 *-------------------------
 * send
 * 03 0c 8e 00 22 31 02 00
 * |  |  |     |     |
 * |  |  value index length
 * |  request
 * requesttype
 *
 * receive no buttons:
 * 3f 55
 * or
 * bf 55
 */
int
read_button_status(usb_dev_handle *deviceHandle) {
//  cout << "Checking status of Canoscan buttons." << endl;

//  int requesttype = 0xC03;
//  int request = 0x0c;
//  int value = 0x8E;

//  int index = 0x3122;
  char buffer[12] = { 3, 9 };
  int expectedLength = 2;
  int timeout = 1000;

  int receivedLength = 0;

  // this is where the magic happens
  // send request to USB device
  // and receive response in buffer
/*  receivedLength = usb_control_msg(
    deviceHandle, requesttype, request, value,
    index, buffer, expectedLength, timeout);
*/
//int usb_bulk_write(usb_dev_handle, int ep, char *bytes, int size, int timeout);
  receivedLength = usb_bulk_write(
    deviceHandle, 1, buffer, expectedLength, timeout);
printf(receivedLength);
  if (receivedLength != 2) {
    cerr << "received length is wrong !!! try again." << endl;
  }

/*
  // print response
  cout << "received length for status query: "
       << receivedLength << endl;
  printf("status: %02x:%02x\n",
    (unsigned char)buffer[0],
    (unsigned char)buffer[1]);
*/

  return ((buffer[0] & 0x7F) * 256) + buffer[1];
}

int
main(void) {
  cout << "r100 test." << endl;
  struct usb_device *r100USBDevice = findUSBDevice();
  usb_dev_handle *deviceHandle = NULL;
  deviceHandle = get_handle(r100USBDevice);

  #define NO_BUTTONS 0x3f55
  #define AUTOSCAN_BUTTON 0x3d55
  #define PDF_BUTTON_1 0x2f55
  #define PDF_BUTTON_2 0x3e55
  #define COPY_BUTTON 0x3b55
  #define EMAIL_BUTTON 0x3755

  int button = NO_BUTTONS;
  while (button == NO_BUTTONS) {
//    cout << "No button pressed." << endl;
    button = read_button_status(deviceHandle);

    // check for button pressed every 0,2 seconds
    usleep(200000);
  }

  if (button == AUTOSCAN_BUTTON) {
    cout << "autoscan button pressed" << endl;
    return 101;
  } else if (button == PDF_BUTTON_1) {
    cout << "pdf button 1 pressed" << endl;
    return 102;
  } else if (button == PDF_BUTTON_2) {
    cout << "pdf button 2 pressed" << endl;
    return 103;
  } else if (button == COPY_BUTTON) {
    cout << "copy button pressed" << endl;
    return 104;
  } else if (button == EMAIL_BUTTON) {
    cout << "email button pressed" << endl;
    return 105;
  }

  return 0;
}
