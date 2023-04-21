#include <usbd.h>

#include "callbacks.h"
#include "shared.h"

int usbmouse_attach(int devId) {
    int result;                     // $v0
    int configEndpoint;             // $v0
    int dataEndpoint;               // $v0
    int gp0;                        // $gp
    int v5;                         // $v0
    int gp2;                        // $gp
    int gp1;                        // $gp
    UsbConfigDescriptor *cdesc;     // [sp+18h] [+18h]
    UsbInterfaceDescriptor *idesc;  // [sp+1Ch] [+1Ch]
    UsbDeviceDescriptor *hdesc;     // [sp+20h] [+20h]
    UsbEndpointDescriptor *edesc;   // [sp+24h] [+24h]
    shared_t *g;                    // [sp+28h] [+28h]
    int maxPacketSize;              // [sp+2Ch] [+2Ch]
    int getDeviceLocationRet;       // [sp+30h] [+30h]
    int i;                          // [sp+30h] [+30h]
    int j;                          // [sp+30h] [+30h]
    int transferPipeRet;            // [sp+30h] [+30h]
    char deviceLocationPath[8];     // [sp+38h] [+38h] BYREF
    UsbDeviceRequest deviceRequest; // [sp+40h] [+40h] BYREF

    cdesc = sceUsbdScanStaticDescriptor(devId, 0, USB_DT_CONFIG);
    if (cdesc) {
        idesc = sceUsbdScanStaticDescriptor(devId, cdesc, USB_DT_INTERFACE);
        if (idesc) {
            if (idesc->bNumEndpoints == 1) {
                edesc = sceUsbdScanStaticDescriptor(devId, idesc, USB_DT_ENDPOINT);
                if (edesc) {
                    hdesc = sceUsbdScanStaticDescriptor(devId, 0, 33);
                    if (hdesc) {
                        if ((edesc->bEndpointAddress & 0x80) != 0) {
                            if ((edesc->bmAttributes & 3) == 3) {
                                maxPacketSize =
                                    edesc->wMaxPacketSizeLB | (edesc->wMaxPacketSizeHB << 8);
                                if (maxPacketSize < 9) {
                                    g = unit_alloc(devId,
                                                   maxPacketSize,
                                                   idesc->bInterfaceNumber,
                                                   idesc->bAlternateSetting);
                                    if (g) {
                                        configEndpoint = sceUsbdOpenPipe(devId, 0);
                                        g->configEndpoint = configEndpoint;
                                        if (configEndpoint >= 0) {
                                            printf("#### sceUsbdOpenPipe %d\n", g->configEndpoint);
                                            dataEndpoint = sceUsbdOpenPipe(devId, edesc);
                                            g->dataEndpoint = dataEndpoint;
                                            if (dataEndpoint >= 0) {
                                                sceUsbdSetPrivateData(devId, g);
                                                g->descriptorChecksPassed = 1;
                                                if (cdesc->bLength != 9 ||
                                                    LOBYTE(cdesc->wTotalLength) != 0x22 ||
                                                    HIBYTE(cdesc->wTotalLength) ||
                                                    cdesc->bNumInterfaces != 1 ||
                                                    cdesc->bConfigurationValue != 1 ||
                                                    cdesc->iConfiguration ||
                                                    cdesc->bmAttributes != 0x80 ||
                                                    cdesc->maxPower != 0xC8) {
                                                    g->descriptorChecksPassed = 0;
                                                    printf("cdesc check failed\n");
                                                }
                                                if (hdesc->bLength != 9 ||
                                                    hdesc->bDescriptorType != 0x21 ||
                                                    LOBYTE(hdesc->bcdUSB) != 0x10 ||
                                                    HIBYTE(hdesc->bcdUSB) != 1 ||
                                                    hdesc->bDeviceClass ||
                                                    hdesc->bDeviceSubClass != 1 ||
                                                    hdesc->bDeviceProtocol != 0x22 ||
                                                    hdesc->bMaxPacketSize0 != 0x52 ||
                                                    LOBYTE(hdesc->idVendor)) {
                                                    g->descriptorChecksPassed = 0;
                                                    printf("hdesc check failed\n");
                                                }
                                                if (edesc->bEndpointAddress != 0x81 ||
                                                    edesc->bmAttributes != 3 ||
                                                    edesc->wMaxPacketSizeLB != 8 ||
                                                    edesc->wMaxPacketSizeHB ||
                                                    LOBYTE(edesc->bInterval) != 4) {
                                                    g->descriptorChecksPassed = 0;
                                                    printf("edesc check failed\n");
                                                }
                                                getDeviceLocationRet = sceUsbdGetDeviceLocation(
                                                    devId, deviceLocationPath);
                                                if (getDeviceLocationRet) {
                                                    printf("usbmouse%d: %s -> 0x%x\n",
                                                           g->port1,
                                                           "sceUsbdGetDeviceLocation",
                                                           getDeviceLocationRet);
                                                    result = -1;
                                                } else {
                                                    printf("usbmouse%d: attached (port=", g->port1);
                                                    for (i = 0; i < 7 && deviceLocationPath[i];
                                                         ++i) {
                                                        if (i)
                                                            v5 = gp0 + 0xFFFF8044;
                                                        else
                                                            v5 = gp0 + 0xFFFF8048;
                                                        printf((const char *)(gp0 + 0xFFFF803C),
                                                               v5,
                                                               deviceLocationPath[i]);
                                                    }
                                                    printf((const char *)(gp0 + 0xFFFF804C));
                                                    printf((const char *)(gp2 + 0xFFFF8020));
                                                    for (j = 0; j < 7; ++j)
                                                        printf((const char *)(gp1 + 0xFFFF8050),
                                                               deviceLocationPath[j]);
                                                    printf((const char *)(gp1 + 0xFFFF8020));
                                                    if (deviceLocationPath[0] == 1) {
                                                        g->port |= 2u;
                                                        printf("attached port = 1\n");
                                                    } else if (deviceLocationPath[0] == 2) {
                                                        g->port |= 1u;
                                                        printf("attached port = 2\n");
                                                    }
                                                    printf("c_pipe = %d\n", g->configEndpoint);
                                                    deviceRequest.requesttype = 0; // USB_DIR_OUT
                                                    deviceRequest.request =
                                                        9; // USB_REQ_SET_CONFIGURATION
                                                    deviceRequest.value =
                                                        cdesc->bConfigurationValue;
                                                    deviceRequest.index = 0;
                                                    deviceRequest.length = 0;
                                                    transferPipeRet =
                                                        sceUsbdTransferPipe( // sceUsbdSetConfiguration(g->c_pipe, cdesc->bConfigurationValue, set_config_done, g)
                                                            g->configEndpoint,
                                                            0,
                                                            0,
                                                            &deviceRequest,
                                                            set_config_done,
                                                            g);
                                                    if (transferPipeRet) {
                                                        printf("usbmouse%d: %s -> 0x%x\n",
                                                               g->port1,
                                                               "sceUsbdSetConfiguration",
                                                               transferPipeRet);
                                                        result = -1;
                                                    } else {
                                                        dump_desc(devId);
                                                        result = 0;
                                                    }
                                                }
                                            } else {
                                                printf("sceUsbdOpenPipe failed(2)\n");
                                                unit_free(g);
                                                result = -1;
                                            }
                                        } else {
                                            printf("sceUsbdOpenPipe failed(1)\n");
                                            result = -1;
                                        }
                                    } else {
                                        printf("unit_alloc failed\n");
                                        result = -1;
                                    }
                                } else {
                                    printf("wrong payload size %d\n", maxPacketSize);
                                    result = -1;
                                }
                            } else {
                                result = -1;
                            }
                        } else {
                            result = -1;
                        }
                    } else {
                        printf("sceUsbdScanStaticDescriptor (HID(0x21)) failed\n");
                        result = -1;
                    }
                } else {
                    printf("sceUsbdScanStaticDescriptor (ENDPOINT) failed\n");
                    result = -1;
                }
            } else {
                printf("wrong bNumEndpoints : %d\n", idesc->bNumEndpoints);
                result = -1;
            }
        } else {
            printf("sceUsbdScanStaticDescriptor (INTERFACE) failed\n");
            result = -1;
        }
    } else {
        printf("sceUsbdScanStaticDescriptor (CONFIGURATION) failed\n");
        result = -1;
    }
    return result;
}
