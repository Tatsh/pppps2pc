#include <usbd.h>

#ifdef DEBUG
#include <stdio.h>
#endif

int usbmouse_probe(int devId) {
    UsbDeviceDescriptor *hdesc;    // [sp+10h] [+10h]
    UsbInterfaceDescriptor *idesc; // [sp+14h] [+14h]

#ifdef DEBUG
    (void) printf("-------- dev_id = %d\n", devId);
#endif

    if ((hdesc = sceUsbdScanStaticDescriptor(devId, 0, 1)) == NULL) {
#ifdef DEBUG
        (void) printf("#### USB : ScanStaticDescriptor Failed(0)\n");
#endif
        goto fail;
    }

    if ((idesc = sceUsbdScanStaticDescriptor(devId, hdesc, 4)) == NULL) {
#ifdef DEBUG
        (void) printf("#### USB : ScanStaticDescriptor Failed(1)\n");
#endif
        goto fail;
    }

    if ((hdesc->bLength != 18 || hdesc->bDescriptorType != 1 || hdesc->bcdUSB != 272 ||
        hdesc->bDeviceClass != 0 || hdesc->bDeviceSubClass >= 1 || hdesc->bDeviceProtocol != 0 ||
        hdesc->bMaxPacketSize0 != 8 || hdesc->idVendor != 1287 || hdesc->idProduct != 17 ||
        hdesc->iManufacturer != 1 || hdesc->iProduct != 2 || hdesc->iSerialNumber >= 1 ||
        hdesc->bNumConfigurations != 1)) {
#ifdef DEBUG
        (void) printf("Wrong Device Desc.\n");
#endif
        goto fail;
    }

    if (idesc->bLength != 9 || idesc->bDescriptorType != 4 ||
        idesc->bInterfaceNumber != 0 || idesc->bAlternateSetting != 0 ||
        idesc->bNumEndpoints != 1 || idesc->bInterfaceClass != 3 ||
        idesc->bInterfaceSubClass != 0 || idesc->bInterfaceProtocol != 0 ||
        idesc->iInterface != 2) {
#ifdef DEBUG
        (void) printf("Wrong Interface Desc.\n");
#endif
        goto fail;
    }

#ifdef DEBUG
    (void) printf("USB_PROBE END\n");
#endif

    return 1;

fail:
    return 0;
}
