#include "callbacks.h"

static char report[0x400];
static const char string_desc[] = {0};
static const int off_2C30 = 0x2C30;
static char kb[0x10];

int data_transfer_done(int result, int count, shared_t *g) {
    int i, offset;
    (void) count;

#ifdef DEBUG
    if (result)
        (void) printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdInterruptTransfer", result);
#endif

    if (!result && g->descriptorChecksPassed) {
        g->count += 1;

#ifdef DEBUG
        (void) printf("usbmouse%d: count=%d data=(", g->port1, g->count);
#endif

        for (i = 0; i < 8; ++i) {
            offset = i + 0x7804;

#ifdef DEBUG
           (void) printf("data at offset %i byte=%c", offset, g->data[offset]);
#endif

            if (g->port) {
                // TODO figure out what the driver is doing here.
                // it appears to be a memcpy.
                *(char *)(8 * (g->port - 1) + i + 0x2D20) = g->data[offset];
                kb[8 * g->port - 1] = 63;
            }
        }
    }

    return data_transfer(g);
}

int get_report_done(int result, int count, shared_t *g) {
    int transferPipeRet;
    (void) count;

#ifdef DEBUG
    if (result)
        (void) printf("usbmouse%d: %s -> 0x%x\n", g->port1, "dump_report", result);
#else
    (void) result;
#endif

#ifdef DEBUG
    int i;
    // TODO Figure out which values are being compared to detect a descriptor check failure.
    for (i = 0; i < count; ++i) {
        if (*(unsigned char *)(i + 11316) != *(unsigned char *)(i + 0x2D30)) {
            (void) printf("report desc error\n");
            g->descriptorChecksPassed = 0;
            break;
        }
    }
#endif

#ifdef DEBUG
    int gp0, j;
    // TODO figure out what its attempting to print out.
    (void) printf("--Report Descriptor(%d)--\n", count);
    for (j = 0; j < count; ++j) {
        (void) printf((const char *)(gp0 + 0xFFFF802C), *(unsigned char *)(j + 0x2D30));
        if (j % 2 == 1)
            (void) printf((const char *)(gp0 + 0xFFFF8034));
        if (j % 16 == 15)
            (void) printf((const char *)(gp0 + 0xFFFF8020));
    }
#endif

    // sceUsbdControlTransfer(g->configEndpoint, 0x80, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) | index, 0x800, ..., g->buf, get_string_done, g)
    if ((transferPipeRet = sceUsbdTransferPipe(g->configEndpoint, g->buf, 0x800, &REQ_GET_REPORT, get_string_done, g))) {
#ifdef DEBUG
        (void) printf("usbmouse%d: sceUsbdSetInterface -> 0x%x\n", g->port1, transferPipeRet)
#endif
    }

    return transferPipeRet;
}

int get_string_done(int result, int count, shared_t *g) {
    int setInterfaceRet;
    unsigned char *v7;    // [sp+1Ch] [+1Ch]
    short *v11;           // [sp+28h] [+28h]
    UsbDeviceRequest req; // [sp+30h] [+30h] BYREF
    (void) count;

    v7 = (unsigned char *)&g->buf[0x2000 * g->unk7];
    if (result) {
        req.requesttype = 1;
        req.request = USB_REQ_SET_PROTOCOL;
        req.value = g->alternateSetting;
        req.index = g->interfaceNumber;
        req.length = 0;

        // sceUsbdSetInterface(g->c_pipe, g->interfaceNumber, g->alternateSetting, set_interface_done, g)
        if ((setInterfaceRet = sceUsbdTransferPipe(g->configEndpoint, 0, 0, &req, set_interface_done, g))) {
#ifdef DEBUG
            (void) printf("usbmouse%d: sceUsbdSetInterface -> 0x%x\n", g->port1, setInterfaceRet)
#endif
        }

        return setInterfaceRet;
    }

#ifdef DEBUG
    int i;
    (void) printf("---StringDesc---\n");
    (void) printf("bDescriptorType 0x%02X\n", v7[1]);
    (void) printf("bString ");

    for (i = 0; i < *v7 - 2; ++i)
        (void) printf("current byte=(%c)\n", v7[i + 2]);
#endif

    switch (g->unk7) {
    case 0: {
        g->data[0x7800] = v7[2] | (v7[3] << 8);
#ifdef DEBUG
        (void) printf("lang_id = 0x%04x\n", *(unsigned short *)&g->data[0x7800]);
#endif
        break;
    }
    case 1:
    case 2: {
        v11 = (short *)*(&string_desc + g->unk7);
        break;
    }
    default: {
        v11 = off_2C30;
        break;
    }
    }

#ifdef DEBUG
    int k;
    // TODO figure out the value being compared for a descriptor failure.
    for (k = 0; k < *v7 - 2; ++k) {
        if (*(unsigned char *)v11 != v7[k + 2]) {
            g->descriptorChecksPassed = 0;
            (void) printf("String Desc Error\n");
        }
        v11 = (short *)((char *)v11 + 1);
    }
#endif

    req.requesttype = USB_DIR_IN;
    req.request = USB_REQ_GET_DESCRIPTOR;
    req.value = LOWORD(g->unk7++) | 0x300;
    req.index = g->data[0x7800];
    req.length = 2048;
    return sceUsbdTransferPipe(g->configEndpoint, &g->buf[0x2000 * g->unk7], 2048, &req, get_string_done, g);
}

int led_transfer_done(int result, int count, shared_t *g) {
#ifdef DEBUG
    if (result)
        (void) printf("usbmouse%d: %s -> 0x%x\n", g->port1, "led_transfer_done", result);
#else
    (void) result;
#endif
    (void) count;
    return data_transfer(g);
}

int set_config_done(int result, int count, shared_t *g) {
    int transferPipeRet;
    (void) count;

#ifdef DEBUG
    if (result)
        (void) printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdSetConfiguration", result);
#else
    (void) result;
#endif

    if ((transferPipeRet = sceUsbdTransferPipe(g->configEndpoint, report, 0x400, &req, get_report_done, g))) {
#ifdef DEBUG
        (void) printf("usbmouse%d: sceUsbdTransferPipe -> 0x%x\n", g->port1, transferPipeRet)
#endif
    }

    return transferPipeRet;
}

int set_interface_done(int result, int count, shared_t *g) {
    UsbDeviceRequest req;
    int transferPipeRet;
    (void) count;

#ifdef DEBUG
    if (result)
        (void) printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdSetInterface", result);
#else
    (void) result;
#endif

    data_transfer(g);
    req.requesttype = USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE;
    req.request = SET_REPORT;
    req.value = 3 << 8;
    req.index = g->interfaceNumber;
    req.length = 1;

    if ((transferPipeRet = sceUsbdTransferPipe(g->configEndpoint, &g->unk5, 1, &req, led_transfer_done, g))) {
#ifdef DEBUG
        (void) printf("usbmouse%d: req-feature -> 0x%x\n", g->port1, transferPipeRet);
#endif
    }

    return transferPipeRet;
}
