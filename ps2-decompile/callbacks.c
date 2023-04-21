#include <usbd.h>

#include "globals.h"
#include "shared.h"

int data_transfer_done(int result, int count, shared_t *g) {
    int usbmouse_count; // $a2
    int v4;             // $gp
    int i;              // [sp+14h] [+14h]

    if (result)
        printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdInterruptTransfer", result);
    if (!result && g->descriptorChecksPassed) {
        usbmouse_count = g->count + 1;
        g->count = usbmouse_count;
        printf("usbmouse%d: count=%d data=(", g->port1, usbmouse_count);
        for (i = 0; i < 8; ++i) {
            // v4 - 32752 = 2CCC
            printf((const char *)(v4 + 0xFFFF8010), g->unk8[i + 0x7804]);
            if (g->port) {
                *(char *)(8 * (g->port - 1) + i + 0x2D20) = g->unk8[i + 0x7804];
                kb[8 * g->port - 1] = 63;
            }
        }
        // v4 - 32744 = 2CD4
        printf((const char *)(v4 + 0xFFFF8018));
    }
    return data_transfer(g);
}

int get_report_done(int result, int count, shared_t *g) {
    int gp0;              // $gp
    int transferPipeRet;  // $v0
    int i;                // [sp+20h] [+20h]
    int j;                // [sp+20h] [+20h]
    UsbDeviceRequest req; // [sp+28h] [+28h] BYREF

    if (result)
        printf("usbmouse%d: %s -> 0x%x\n", g->port1, "dump_report", result);
    for (i = 0; i < count; ++i) {
        if (*(unsigned char *)(i + 11316) != *(unsigned char *)(i + 0x2D30)) {
            printf("report desc error\n");
            g->descriptorChecksPassed = 0;
            break;
        }
    }
    printf("--Report Descriptor(%d)--\n", count);
    for (j = 0; j < count; ++j) {
        printf((const char *)(gp0 + 0xFFFF802C), *(unsigned char *)(j + 0x2D30));
        if (j % 2 == 1)
            printf((const char *)(gp0 + 0xFFFF8034));
        if (j % 16 == 15)
            printf((const char *)(gp0 + 0xFFFF8020));
    }
    printf((const char *)(gp0 + 0xFFFF8020));
    req.requesttype = 0x80; // USB_DIR_IN
    req.request = 6;        // USB_REQ_GET_DESCRIPTOR
    req.value = 768;
    req.index = 0;
    req.length = 0x800;
    transferPipeRet = sceUsbdTransferPipe(
        g->configEndpoint,
        g->buf,
        0x800,
        &req,
        get_string_done,
        g); // sceUsbdControlTransfer(g->configEndpoint, 0x80, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) | index, 0x800, ..., g->buf, get_string_done, g)
    if (transferPipeRet)
        transferPipeRet =
            printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdSetInterface", transferPipeRet);
    return transferPipeRet;
}

const char string_desc[] = {0};
const int off_2C30 = 0x2C30;

int get_string_done(int result, int count, shared_t *g) {
    int v3;               // $gp
    int v4;               // $gp
    int setInterfaceRet;  // $v0
    unsigned char *v7;    // [sp+1Ch] [+1Ch]
    int i;                // [sp+20h] [+20h]
    int j;                // [sp+20h] [+20h]
    int k;                // [sp+20h] [+20h]
    short *v11;           // [sp+28h] [+28h]
    UsbDeviceRequest req; // [sp+30h] [+30h] BYREF

    v7 = (unsigned char *)&g->buf[0x2000 * g->unk7];
    if (result) {
        req.requesttype = 1;
        req.request = 0xB; // USB_REQ_SET_PROTOCOL
        req.value = g->alternateSetting;
        req.index = g->interfaceNumber;
        req.length = 0;
        setInterfaceRet = sceUsbdTransferPipe(
            g->configEndpoint,
            0,
            0,
            &req,
            set_interface_done,
            g); // sceUsbdSetInterface(g->c_pipe, g->interfaceNumber, g->alternateSetting, set_interface_done, g)
        if (setInterfaceRet)
            setInterfaceRet = printf(
                "usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdSetInterface", setInterfaceRet);
    } else {
        printf("---StringDesc---\n");
        printf("bDescriptorType 0x%02X\n", v7[1]);
        printf("bString ");
        for (i = 0; i < *v7 - 2; ++i)
            printf((const char *)(v3 + 0xFFFF801C), v7[i + 2]);
        printf((const char *)(v3 + 0xFFFF8020));
        for (j = 0; j < *v7 - 2; ++j) {
            printf((const char *)(v4 + 0xFFFF8024), v7[j + 2]);
            if (j % 16 == 15)
                printf((const char *)(v4 + 0xFFFF8020));
        }
        printf((const char *)(v4 + 0xFFFF8020));
        if (!g->unk7) {
            *(int *)&g->unk8[0x7800] = v7[2] | (v7[3] << 8);
            printf("lang_id = 0x%04x\n", *(unsigned short *)&g->unk8[0x7800]);
        }
        if (g->unk7 < 3)
            v11 = (short *)*(&string_desc + g->unk7);
        else
            v11 = off_2C30;
        for (k = 0; k < *v7 - 2; ++k) {
            if (*(unsigned char *)v11 != v7[k + 2]) {
                g->descriptorChecksPassed = 0;
                printf("String Desc Error\n");
            }
            v11 = (short *)((char *)v11 + 1);
        }
        ++g->unk7;
        req.requesttype = 0x80; // USB_DIR_IN
        req.request = 6;        // USB_REQ_GET_DESCRIPTOR
        req.value = LOWORD(g->unk7) | 0x300;
        req.index = *(int *)&g->unk8[0x7800];
        req.length = 2048;
        setInterfaceRet = sceUsbdTransferPipe(
            g->configEndpoint, &g->buf[0x2000 * g->unk7], 2048, &req, get_string_done, g);
    }
    return setInterfaceRet;
}

int led_transfer_done(int result, int count, shared_t *g) {
    if (result)
        printf("usbmouse%d: %s -> 0x%x\n", g->port1, "led_transfer_done", result);
    return data_transfer(g);
}

char report[0x400];

int set_config_done(int result, int count, shared_t *g) {
    int transferPipeRet;  // $v0
    int gp0;              // $gp
    UsbDeviceRequest req; // [sp+20h] [+20h] BYREF

    if (result)
        printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdSetConfiguration", result);
    req.requesttype = 0x81; // USB_DIR_OUT | USB_RECIP_DEVICE
    req.request = 6;        // USB_REQ_GET_DESCRIPTOR
    req.value = 0x2200;
    req.index = 0;
    req.length = 0x400;
    transferPipeRet =
        sceUsbdTransferPipe(g->configEndpoint, report, 0x400, &req, get_report_done, g);
    if (transferPipeRet)
        // v4 - 32712 = 2CF4
        transferPipeRet = printf("usbmouse%d: %s -> 0x%x\n",
                                 g->port1,
                                 (const char *)(gp0 + 0xFFFF8038),
                                 transferPipeRet);
    return transferPipeRet;
}

int set_interface_done(int result, int count, shared_t *g) {
    int transferPipeRet;  // $v0
    UsbDeviceRequest req; // [sp+20h] [+20h] BYREF

    if (result)
        printf("usbmouse%d: %s -> 0x%x\n", g->port1, "sceUsbdSetInterface", result);
    data_transfer(g);
    req.requesttype = 0x21; // USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE
    req.request = 9;        // SET_REPORT
    req.value = 0x300;      // 3 << 8
    req.index = g->interfaceNumber;
    req.length = 1;
    transferPipeRet =
        sceUsbdTransferPipe(g->configEndpoint, &g->unk5, 1, &req, led_transfer_done, g);
    if (transferPipeRet)
        transferPipeRet =
            printf("usbmouse%d: %s -> 0x%x\n", g->port1, "req-feature", transferPipeRet);
    return transferPipeRet;
}
