#include "driver.h"

static char dword_2D18[2048];

int usbmouse_attach(int devId) {
    int maxPacketSize, getDeviceLocationRet, transferPipeRet;
    UsbConfigDescriptor *cdesc;     // [sp+18h] [+18h]
    UsbInterfaceDescriptor *idesc;  // [sp+1Ch] [+1Ch]
    UsbDeviceDescriptor *hdesc;     // [sp+20h] [+20h]
    UsbEndpointDescriptor *edesc;   // [sp+24h] [+24h]
    shared_t *g;                    // [sp+28h] [+28h]
    char deviceLocationPath[8];     // [sp+38h] [+38h] BYREF
    UsbDeviceRequest deviceRequest; // [sp+40h] [+40h] BYREF

    memset(deviceLocationPath, 0, sizeof(deviceLocationPath));

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

    if ((g->configEndpoint = sceUsbdOpenPipe(devId, 0)) < USB_RC_OK) {
#ifdef DEBUG
        printf("sceUsbdOpenPipe failed(1)\n");
#endif
        goto fail;
    }
#ifdef DEBUG
    printf("#### sceUsbdOpenPipe %d\n", g->configEndpoint);
#endif

    if ((g->dataEndpoint = sceUsbdOpenPipe(devId, edesc)) < USB_RC_OK) {
#ifdef DEBUG
        printf("sceUsbdOpenPipe failed(2)\n");
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

#ifdef DEBUG
    dump_desc(devId);
#endif
    return 0;

fail:
    if (g)
        unit_free(g);
    return -1;
}

int usbmouse_detach(int devId) {
    shared_t *p; // [sp+10h] [+10h]

    if ((p = sceUsbdGetPrivateData(devId)) == NULL)
        return -1;

    if (p->port > 0)
        memset(&dword_2D18[2 * p->port], 0, 8u);

    unit_free(p);

#ifdef DEBUG
    printf("usbmouse%d: detached : port %d\n", p->port1, p->port);
#endif

    return 0;
}

int usbmouse_probe(int devId) {
    UsbDeviceDescriptor *hdesc;    // [sp+10h] [+10h]
    UsbInterfaceDescriptor *idesc; // [sp+14h] [+14h]

#ifdef DEBUG
    printf("-------- dev_id = %d\n", devId);
#endif

    if ((hdesc = sceUsbdScanStaticDescriptor(devId, 0, 1)) == NULL) {
#ifdef DEBUG
        printf("#### USB : ScanStaticDescriptor Failed(0)\n");
#endif
        goto fail;
    }

    if ((idesc = sceUsbdScanStaticDescriptor(devId, hdesc, 4)) == NULL) {
#ifdef DEBUG
        printf("#### USB : ScanStaticDescriptor Failed(1)\n");
#endif
        goto fail;
    }

    if ((hdesc->bLength != 18 || hdesc->bDescriptorType != 1 || hdesc->bcdUSB != 272 ||
        hdesc->bDeviceClass != 0 || hdesc->bDeviceSubClass >= 1 || hdesc->bDeviceProtocol != 0 ||
        hdesc->bMaxPacketSize0 != 8 || hdesc->idVendor != 1287 || hdesc->idProduct != 17 ||
        hdesc->iManufacturer != 1 || hdesc->iProduct != 2 || hdesc->iSerialNumber >= 1 ||
        hdesc->bNumConfigurations != 1)) {
#ifdef DEBUG
        printf("Wrong Device Desc.\n");
#endif
        goto fail;
    }

    if (idesc->bLength != 9 || idesc->bDescriptorType != 4 ||
        idesc->bInterfaceNumber != 0 || idesc->bAlternateSetting != 0 ||
        idesc->bNumEndpoints != 1 || idesc->bInterfaceClass != 3 ||
        idesc->bInterfaceSubClass != 0 || idesc->bInterfaceProtocol != 0 ||
        idesc->iInterface != 2) {
#ifdef DEBUG
        printf("Wrong Interface Desc.\n");
#endif
        goto fail;
    }

#ifdef DEBUG
    printf("USB_PROBE END\n");
#endif

    return 1;

fail:
    return 0;
}
