#include "utils.h"

int data_transfer(shared_t *g) {
    int result;

    if ((result = sceUsbdInterruptTransfer(g->dataEndpoint, &g->data[0x7804], g->maxPacketSize, 0, data_transfer_done, g))) {
#ifdef DEBUG
        (void) printf("usbmouse%d: sceUsbdInterruptTransfer -> 0x%x\n", g->port1, result)
#endif
    }

    return result;
}

#ifdef DEBUG
UsbDeviceDescriptor *dump_desc(int devId) {
    UsbDeviceDescriptor *result; // $v0
    UsbConfigDescriptor *cdesc;  // [sp+18h] [+18h]
    UsbInterfaceDescriptor *idesc;
    UsbEndpointDescriptor *edesc;
    UsbStringDescriptor *sdesc;

    (void) printf("\n\nUSBKB : static Descriptor DUMP\n");

    while (1) {
        // 0 means go through all descriptors
        if ((result = sceUsbdScanStaticDescriptor(devId, cdesc, 0)) == NULL)
            break;

        switch (result->bDescriptorType) {
        case USB_DT_DEVICE:
            (void) printf("--Device--\n");
            (void) printf("bLength 0x%02X\n", result->bLength);
            (void) printf("bDescriptorType 0x%02X\n", result->bDescriptorType);
            (void) printf("bcdUSB 0x%02X\n", result->bcdUSB);
            (void) printf("bDeviceClass 0x%02X\n", result->bDeviceClass);
            (void) printf("bDeviceSubClass 0x%02X\n", result->bDeviceSubClass);
            (void) printf("bDeviceProtocol 0x%02X\n", result->bDeviceProtocol);
            (void) printf("bMaxPacketSize0 0x%02X\n", result->bMaxPacketSize0);
            (void) printf("idVendor 0x%02X\n", result->idVendor);
            (void) printf("idProduct 0x%02X\n", result->idProduct);
            (void) printf("bcdDevice 0x%02X\n", result->bcdDevice);
            (void) printf("iManufacturer 0x%02X\n", result->iManufacturer);
            (void) printf("iProduct 0x%02X\n", result->iProduct);
            (void) printf("iSerialNumber 0x%02X\n", result->iSerialNumber);
            (void) printf("bNumConfigurations 0x%02X\n", result->bNumConfigurations);
            break;
        case USB_DT_CONFIG:
            cdesc = result;
            (void) printf("---ConfigDesc---\n");
            (void) printf("bLength 0x%02X\n", cdesc->bLength);
            (void) printf("wTotalLength0 0x%02X\n", LOBYTE(cdesc->wTotalLength));
            (void) printf("wTotalLength1 0x%02X\n", HIBYTE(cdesc->wTotalLength));
            (void) printf("bNumInterfaces 0x%02X\n", cdesc->bNumInterfaces);
            (void) printf("bConfigurationValue 0x%02X\n", cdesc->bConfigurationValue);
            (void) printf("iConfiguration 0x%02X\n", cdesc->iConfiguration);
            (void) printf("bmAttribute 0x%02X\n", cdesc->bmAttributes);
            (void) printf("MaxPower 0x%02X\n", cdesc->maxPower);
            break;
        case USB_DT_STRING:
            sdesc = result;
            (void) printf("---StringDesc---\n");
            (void) printf("bDescriptorType 0x%02X\n", sdesc->bDescriptorType);
            (void) printf("bString ");
            (void) printf("wData[0] = 0x%04x", sdesc->wData[0]);
            break;
        case USB_DT_INTERFACE:
            idesc = result;
            (void) printf("--Interface--\n");
            (void) printf("bLength 0x%02X\n", idesc->bLength);
            (void) printf("bDescriptorType 0x%02X\n", idesc->bDescriptorType);
            (void) printf("bInterfaceNumber 0x%02X\n", idesc->bInterfaceNumber);
            (void) printf("bAlternateSetting 0x%02X\n", idesc->bAlternateSetting);
            (void) printf("bNumEndpoints 0x%02X\n", idesc->bNumEndpoints);
            (void) printf("bInterfaceClass 0x%02X\n", idesc->bInterfaceClass);
            (void) printf("bInterfaceSubClass 0x%02X\n", idesc->bInterfaceSubClass);
            (void) printf("bInterfaceProtocol 0x%02X\n", idesc->bInterfaceProtocol);
            (void) printf("iInterface 0x%02X\n", idesc->iInterface);
            break;
        case USB_DT_ENDPOINT:
            edesc = result;
            (void) printf("ENDPOINT ADR 0x%02X ATR 0x%02X SIZE 0x%02X INTR 0x%02X\n",
                   LOBYTE(edesc->bEndpointAddress),
                   HIBYTE(edesc->bEndpointAddress),
                   edesc->wMaxPacketSizeHB | (edesc->wMaxPacketSizeLB << 8),
                   edesc->bInterval);
            break;
        default:
            // if (result->bDescriptorType == 33) {
            //     (void) printf("--HID--\n");
            //     (void) printf("bLength 0x%02X\n", cdesc->bLength);
            //     (void) printf("bDescriptorType 0x%02X\n", cdesc->bDescriptorType);
            //     (void) printf("bcdUSB 0x%02X\n",
            //            (HIBYTE(cdesc->wTotalLength) << 8) | LOBYTE(cdesc->wTotalLength));
            //     (void) printf("bCountryCode 0x%02X\n", cdesc->bNumInterfaces);
            //     (void) printf("bNumDescriptors 0x%02X\n", cdesc->bConfigurationValue);
            //     (void) printf("bDescriptorType2 0x%02X\n", cdesc->iConfiguration);
            //     (void) printf("wDescriptorLength0 0x%02X\n", cdesc->bmAttributes);
            //     (void) printf("wDescriptorLength1 0x%02X\n", cdesc->maxPower);
            // } else {
            //     (void) printf("--Unknown--\n");
            //     (void) printf("bLength 0x%02X\n", cdesc->bLength);
            //     (void) printf("bDescriptorType 0x%02X\n", cdesc->bDescriptorType);
            // }
            break;
        }
    }
    return result;
}
#endif
