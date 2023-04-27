#include <usbd.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "callbacks.h"
#include "globals.h"
#include "shared.h"
#include "utils.h"

int usbmouse_attach(int devId) {
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
        (void) printf("sceUsbdScanStaticDescriptor (CONFIGURATION) failed\n");
#endif
        goto fail;
    }

    if ((idesc = sceUsbdScanStaticDescriptor(devId, cdesc, USB_DT_INTERFACE)) == NULL) {
#ifdef DEBUG
        (void) printf("sceUsbdScanStaticDescriptor (INTERFACE) failed\n");
#endif
        goto fail;
    }

    if (idesc->bNumEndpoints != USB_ENDPOINT_XFER_ISOC) {
#ifdef DEBUG
        (void) printf("wrong bNumEndpoints : %d\n", idesc->bNumEndpoints);
#endif
        goto fail;
    }

    if ((edesc = sceUsbdScanStaticDescriptor(devId, idesc, USB_DT_ENDPOINT)) == NULL) {
#ifdef DEBUG
        (void) printf("sceUsbdScanStaticDescriptor (ENDPOINT) failed\n");
#endif
        goto fail;
    }

    if ((hdesc = sceUsbdScanStaticDescriptor(devId, 0, 33)) == NULL) {
#ifdef DEBUG
        (void) printf("sceUsbdScanStaticDescriptor (HID(0x21)) failed\n");
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
        (void) printf("wrong payload size %d\n", maxPacketSize);
#endif
        goto fail;
    }

    if ((g = unit_alloc(devId, maxPacketSize,idesc->bInterfaceNumber, idesc->bAlternateSetting)) == NULL) {
#ifdef DEBUG
        (void) printf("unit_alloc failed\n");
#endif
        goto fail;
    }

    if ((g->configEndpoint = sceUsbdOpenPipe(devId, 0)) < USB_RC_OK) {
#ifdef DEBUG
        (void) printf("sceUsbdOpenPipe failed(1)\n");
#endif
        goto fail;
    }
#ifdef DEBUG
    (void) printf("#### sceUsbdOpenPipe %d\n", g->configEndpoint);
#endif

    if ((g->dataEndpoint = sceUsbdOpenPipe(devId, edesc)) < USB_RC_OK) {
#ifdef DEBUG
        (void) printf("sceUsbdOpenPipe failed(2)\n");
#endif
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
        (void) printf("cdesc check %s\n", g->descriptorChecksPassed == 0 ? "failed" : "passed");
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
        (void) printf("hdesc check %s\n", g->descriptorChecksPassed == 0 ? "failed" : "passed");
#endif

    g->descriptorChecksPassed = edesc->bEndpointAddress == USB_DEVICE_ENDPOINT_ADDRESS &&
            edesc->bmAttributes == USB_REQ_SET_FEATURE &&
            edesc->wMaxPacketSizeLB == USB_MAX_PACKET_LO_BYTE &&
            edesc->wMaxPacketSizeHB == USB_MAX_PACKET_HI_BYTE &&
            edesc->bInterval == USB_INTERVAL;

#ifdef DEBUG
        (void) printf("edesc check %s\n", g->descriptorChecksPassed == 0 ? "failed" : "passed");
#endif

    if ((getDeviceLocationRet = sceUsbdGetDeviceLocation(devId, deviceLocationPath))) {
#ifdef DEBUG
        (void) printf("usbmouse%d: sceUsbdGetDeviceLocation -> 0x%x\n", g->port1, getDeviceLocationRet);
#endif
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
    (void) printf("attached port = %i\n", deviceLocationPath[0] == 1 ? 1 : 2);
    (void) printf("c_pipe = %d\n", g->configEndpoint);
#endif

    deviceRequest.request = USB_REQ_SET_CONFIGURATION;
    deviceRequest.requesttype = USB_DIR_OUT;
    deviceRequest.value = cdesc->bConfigurationValue;
    deviceRequest.index = 0;
    deviceRequest.length = 0;

    if ((transferPipeRet = sceUsbdTransferPipe(g->configEndpoint, 0, 0,
                                               &deviceRequest, set_config_done, g))) {
#ifdef DEBUG
        (void) printf("usbmouse%d: sceUsbdSetConfiguration -> 0x%x\n", g->port1, transferPipeRet);
#endif
    }

#ifdef DEBUG
    dump_desc(devId);
#endif

    return 0;

fail:
    if (g)
        unit_free(g);

    return -1;
}
