#include <usbd.h>

#include "callbacks.h"
#include "shared.h"

static const int USB_ID = 0x110;
static const int USB_MAX_POWER = 0xC8;
static const int USB_DESCRIPTOR_TYPE = 0x21;
static const int USB_DEVICE_CLASS = 0x0;
static const int USB_DEVICE_PROTOCOL = 0x22;
static const int USB_DEVICE_PROTOCOL_LENGTH = 0x09;
static const int USB_DEVICE_PROTOCOL_MAX_SIZE = 0x52;
static const int USB_PROTOCOL_TOTAL_LENGTH = 0x2200;
static const int USB_DEVICE_VENDOR = 0x5051;
static const int USB_DEVICE_ENDPOINT_ADDRESS = 0x81;
static const unsigned int USB_PORT_A = 0x02u;
static const unsigned int USB_PORT_B = 0x01u;
static const int USB_MAX_PACKET_LO_BYTE = 0x08;
static const int USB_MAX_PACKET_HI_BYTE = 0x01;

int usbmouse_attach(int devId) {
    int i;
    int gp0, gp1, gp2, v5;           // $gp, $v0
    int maxPacketSize, getDeviceLocationRet, transferPipeRet;
    UsbConfigDescriptor *cdesc;     // [sp+18h] [+18h]
    UsbInterfaceDescriptor *idesc;  // [sp+1Ch] [+1Ch]
    UsbDeviceDescriptor *hdesc;     // [sp+20h] [+20h]
    UsbEndpointDescriptor *edesc;   // [sp+24h] [+24h]
    shared_t *g;                    // [sp+28h] [+28h]
    char deviceLocationPath[8];     // [sp+38h] [+38h] BYREF
    UsbDeviceRequest deviceRequest; // [sp+40h] [+40h] BYREF

    if ((cdesc = sceUsbdScanStaticDescriptor(devId, 0, USB_DT_CONFIG)) == NULL) {
#ifdef DEBUG
        printf("sceUsbdScanStaticDescriptor (CONFIGURATION) failed\n");
#endif
        goto fail;
    }

    if ((idesc = sceUsbdScanStaticDescriptor(devId, cdesc, USB_DT_INTERFACE)) == NULL) {
#ifdef DEBUG
        printf("sceUsbdScanStaticDescriptor (INTERFACE) failed\n");
#endif
        goto fail;
    }

    if (idesc->bNumEndpoints != USB_ENDPOINT_XFER_ISOC) {
#ifdef DEBUG
        printf("wrong bNumEndpoints : %d\n", idesc->bNumEndpoints);
#endif
        goto fail;
    }

    if ((edesc = sceUsbdScanStaticDescriptor(devId, idesc, USB_DT_ENDPOINT)) == NULL) {
#ifdef DEBUG
        printf("sceUsbdScanStaticDescriptor (ENDPOINT) failed\n");
#endif
        goto fail;
    }

    if ((hdesc = sceUsbdScanStaticDescriptor(devId, 0, 33)) == NULL) {
#ifdef DEBUG
        printf("sceUsbdScanStaticDescriptor (HID(0x21)) failed\n");
#endif
        goto fail;
    }

    if ((edesc->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_XFER_CONTROL)
        goto fail;

    if ((edesc->bmAttributes & USB_REQ_SET_FEATURE) != USB_REQ_SET_FEATURE)
        goto fail;

    maxPacketSize = edesc->wMaxPacketSizeLB | (edesc->wMaxPacketSizeHB << 8);

    if (maxPacketSize >= USB_DEVICE_PROTOCOL_LENGTH) {
#ifdef DEBUG
        printf("wrong payload size %d\n", maxPacketSize);
#endif
        goto fail;
    }

    if ((g = unit_alloc(devId, maxPacketSize,idesc->bInterfaceNumber, idesc->bAlternateSetting)) == NULL) {
#ifdef DEBUG
        printf("unit_alloc failed\n");
#endif
        goto fail;
    }

    if ((g->configEnpoint = sceUsbdOpenPipe(devId, 0)) < USB_RC_OK) {
#ifdef DEBUG
        printf("sceUsbdOpenPipe failed(1)\n");
#endif
        unit_free(g);
        goto fail;
    }
#ifdef DEBUG
    printf("#### sceUsbdOpenPipe %d\n", g->configEndpoint);
#endif

    if ((g->dataEndpoint = sceUsbdOpenPipe(devId, edesc)) < USB_RC_OK) {
#ifdef DEBUG
        printf("sceUsbdOpenPipe failed(2)\n");
#endif
        unit_free(g);
        goto fail;
    }

    sceUsbdSetPrivateData(devId, g);

    g->descriptorChecksPassed = cdesc->bLength == USB_DEVICE_PROTOCOL_LENGTH ||
            cdesc->wTotalLength == USB_PROTOCOL_TOTAL_LENGTH ||
            cdesc->bNumInterfaces == USB_DT_DEVICE ||
            cdesc->bConfigurationValue == USB_REQ_CLEAR_FEATURE ||
            cdesc->iConfiguration == USB_REQ_GET_STATUS ||
            cdesc->bmAttributes == USB_DIR_IN ||
            cdesc->maxPower == USB_MAX_POWER;

#ifdef DEBUG
        printf("cdesc check %s\n", g->descriptorChecksPassed == 0 ? "failed" : "passed");
#endif

    g->descriptorChecksPassed = hdesc->bLength == USB_DEVICE_PROTOCOL_LENGTH &&
            hdesc->bDescriptorType == USB_DESCRIPTOR_TYPE &&
            hdesc->bcdUSB == USB_ID &&
            hdesc->bDeviceClass == USB_DIR_OUT &&
            hdesc->bDeviceSubClass == USB_DT_DEVICE ||
            hdesc->bDeviceProtocol == USB_DEVICE_PROTOCOL ||
            hdesc->bMaxPacketSize0 == USB_DEVICE_PROTOCOL_MAX_SIZE ||
            hdesc->idVendor == USB_DEVICE_VENDOR;

#ifdef DEBUG
        printf("hdesc check %s\n", g->descriptorChecksPassed == 0 ? "failed" : "passed");
#endif

    g->descriptorChecksPassed = edesc->bEndpointAddress == USB_DEVICE_ENDPOINT_ADDRESS &&
            edesc->bmAttributes == USB_REQ_SET_FEATURE &&
            edesc->wMaxPacketSizeLB == USB_MAX_PACKET_LO_BYTE &&
            edesc->wMaxPacketSizeHB == USB_MAX_PACKET_HI_BYTE &&
            edesc->bInterval == USB_INTERVAL;

#ifdef DEBUG
        printf("edesc check %s\n", g->descriptorChecksPassed == 0 ? "failed" : "passed");
#endif

    if ((getDeviceLocationRet = sceUsbdGetDeviceLocation(
             devId, deviceLocationPath))) {
#ifdef DEBUG
        printf("usbmouse%d: sceUsbdGetDeviceLocation -> 0x%x\n", g->port1,
               getDeviceLocationRet);
#endif
        unit_free(g);
        goto fail;
    }


    switch (deviceLocationPath[0]) {
    case 1:
        g->port |= USB_PORT_A;
        break;
    case 2:
        g->port |= USB_PORT_B;
        break;
    }

#ifdef DEBUG
    printf("attached port = %i\n", deviceLocationPath[0] == 1 ? 1 : 2);
    printf("c_pipe = %d\n", g->configEndpoint);
#endif

    deviceRequest.request = USB_REQ_SET_CONFIGURATION;
    deviceRequest.requesttype = USB_DIR_OUT;
    deviceRequest.value = cdesc->bConfigurationValue;
    deviceRequest.index = 0;
    deviceRequest.length = 0;

    if ((transferPipeRet = sceUsbdTransferPipe(g->configEndpoint, 0, 0,
                                               &deviceRequest, set_config_done, g))) {
#ifdef DEBUG
        printf("usbmouse%d: sceUsbdSetConfiguration -> 0x%x\n",
               g->port1, transferPipeRet);
#endif
        goto fail;
    }

    dump_desc(devId);
    return 0;

fail:
    if (g)
        unit_free(g);
    return -1;
}
