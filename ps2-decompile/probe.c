#include <stdio.h>
#include <usbd.h>

int usbmouse_probe(int devId) {
    int result;                    // $v0
    UsbDeviceDescriptor *hdesc;    // [sp+10h] [+10h]
    UsbInterfaceDescriptor *idesc; // [sp+14h] [+14h]

    printf("-------- dev_id = %d\n", devId);
    hdesc = sceUsbdScanStaticDescriptor(devId, 0, 1);
    if (hdesc) {
        idesc = sceUsbdScanStaticDescriptor(devId, hdesc, 4);
        if (idesc) {
            if (hdesc->bLength == 18 && hdesc->bDescriptorType == 1 && hdesc->bcdUSB == 272 &&
                !hdesc->bDeviceClass && !hdesc->bDeviceSubClass && !hdesc->bDeviceProtocol &&
                hdesc->bMaxPacketSize0 == 8 && hdesc->idVendor == 1287 && hdesc->idProduct == 17 &&
                hdesc->iManufacturer == 1 && hdesc->iProduct == 2 && !hdesc->iSerialNumber &&
                hdesc->bNumConfigurations == 1) {
                if (idesc->bLength == 9 && idesc->bDescriptorType == 4 &&
                    !idesc->bInterfaceNumber && !idesc->bAlternateSetting &&
                    idesc->bNumEndpoints == 1 && idesc->bInterfaceClass == 3 &&
                    !idesc->bInterfaceSubClass && !idesc->bInterfaceProtocol &&
                    idesc->iInterface == 2) {
                    printf("USB_PROBE END\n");
                    result = 1;
                } else {
                    printf("Wrong Interface Desc.\n");
                    result = 0;
                }
            } else {
                printf("Wrong Device Desc.\n");
                result = 0;
            }
        } else {
            printf("#### USB : ScanStaticDescriptor Failed(1)\n");
            result = 0;
        }
    } else {
        printf("#### USB : ScanStaticDescriptor Failed(0)\n");
        result = 0;
    }
    return result;
}
