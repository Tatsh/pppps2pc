#include "utils.h"

int data_transfer(shared_t *g) {
    int result; // $v0
    result = sceUsbdInterruptTransfer(g->dataEndpoint, &g->unk8[0x7804], g->maxPacketSize, 0, data_transfer_done, g);

#ifdef DEBUG
    if (result)
        result = printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdInterruptTransfer", result);
#endif

    return result;
}

#ifdef DEBUG
UsbDeviceDescriptor *dump_desc(int devId) {
    UsbDeviceDescriptor *result; // $v0
    UsbConfigDescriptor *cdesc;  // [sp+18h] [+18h]
    UsbInterfaceDescriptor *idesc;
    UsbEndpointDescriptor *edesc;
    UsbStringDescriptor *sdesc;

    printf("\n\nUSBKB : static Descriptor DUMP\n");
    while (1) {
        // 0 means go through all descriptors
        if ((result = sceUsbdScanStaticDescriptor(devId, cdesc, 0)) == NULL)
            break;

        switch (result->bDescriptorType) {
        case USB_DT_DEVICE:
            printf("--Device--\n");
            printf("bLength 0x%02X\n", result->bLength);
            printf("bDescriptorType 0x%02X\n", result->bDescriptorType);
            printf("bcdUSB 0x%02X\n", result->bcdUSB);
            printf("bDeviceClass 0x%02X\n", result->bDeviceClass);
            printf("bDeviceSubClass 0x%02X\n", result->bDeviceSubClass);
            printf("bDeviceProtocol 0x%02X\n", result->bDeviceProtocol);
            printf("bMaxPacketSize0 0x%02X\n", result->bMaxPacketSize0);
            printf("idVendor 0x%02X\n", result->idVendor);
            printf("idProduct 0x%02X\n", result->idProduct);
            printf("bcdDevice 0x%02X\n", result->bcdDevice);
            printf("iManufacturer 0x%02X\n", result->iManufacturer);
            printf("iProduct 0x%02X\n", result->iProduct);
            printf("iSerialNumber 0x%02X\n", result->iSerialNumber);
            printf("bNumConfigurations 0x%02X\n", result->bNumConfigurations);
            break;
        case USB_DT_CONFIG:
            cdesc = result;
            printf("---ConfigDesc---\n");
            printf("bLength 0x%02X\n", cdesc->bLength);
            printf("wTotalLength0 0x%02X\n", LOBYTE(cdesc->wTotalLength));
            printf("wTotalLength1 0x%02X\n", HIBYTE(cdesc->wTotalLength));
            printf("bNumInterfaces 0x%02X\n", cdesc->bNumInterfaces);
            printf("bConfigurationValue 0x%02X\n", cdesc->bConfigurationValue);
            printf("iConfiguration 0x%02X\n", cdesc->iConfiguration);
            printf("bmAttribute 0x%02X\n", cdesc->bmAttributes);
            printf("MaxPower 0x%02X\n", cdesc->maxPower);
            break;
        case USB_DT_STRING:
            sdesc = result;
            printf("---StringDesc---\n");
            printf("bDescriptorType 0x%02X\n", sdesc->bDescriptorType);
            printf("bString ");
            printf("wData[0] = 0x%04x", sdesc->wData[0]);
            break;
        case USB_DT_INTERFACE:
            idesc = result;
            printf("--Interface--\n");
            printf("bLength 0x%02X\n", idesc->bLength);
            printf("bDescriptorType 0x%02X\n", idesc->bDescriptorType);
            printf("bInterfaceNumber 0x%02X\n", idesc->bInterfaceNumber);
            printf("bAlternateSetting 0x%02X\n", idesc->bAlternateSetting);
            printf("bNumEndpoints 0x%02X\n", idesc->bNumEndpoints);
            printf("bInterfaceClass 0x%02X\n", idesc->bInterfaceClass);
            printf("bInterfaceSubClass 0x%02X\n", idesc->bInterfaceSubClass);
            printf("bInterfaceProtocol 0x%02X\n", idesc->bInterfaceProtocol);
            printf("iInterface 0x%02X\n", idesc->iInterface);
            break;
        case USB_DT_ENDPOINT:
            edesc = result;
            printf("ENDPOINT ADR 0x%02X ATR 0x%02X SIZE 0x%02X INTR 0x%02X\n",
                   LOBYTE(edesc->bEndpointAddress),
                   HIBYTE(edesc->bEndpointAddress),
                   edesc->wMaxPacketSizeHB | (edesc->wMaxPacketSizeLB << 8),
                   edesc->bInterval);
            break;
        default:
            // if (result->bDescriptorType == 33) {
            //     printf("--HID--\n");
            //     printf("bLength 0x%02X\n", cdesc->bLength);
            //     printf("bDescriptorType 0x%02X\n", cdesc->bDescriptorType);
            //     printf("bcdUSB 0x%02X\n",
            //            (HIBYTE(cdesc->wTotalLength) << 8) | LOBYTE(cdesc->wTotalLength));
            //     printf("bCountryCode 0x%02X\n", cdesc->bNumInterfaces);
            //     printf("bNumDescriptors 0x%02X\n", cdesc->bConfigurationValue);
            //     printf("bDescriptorType2 0x%02X\n", cdesc->iConfiguration);
            //     printf("wDescriptorLength0 0x%02X\n", cdesc->bmAttributes);
            //     printf("wDescriptorLength1 0x%02X\n", cdesc->maxPower);
            // } else {
            //     printf("--Unknown--\n");
            //     printf("bLength 0x%02X\n", cdesc->bLength);
            //     printf("bDescriptorType 0x%02X\n", cdesc->bDescriptorType);
            // }
            break;
        }
    }
    return result;
}
#endif
