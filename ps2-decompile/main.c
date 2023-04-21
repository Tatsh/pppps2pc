#include <sifrpc.h>
#include <thbase.h>
#include <usbd.h>

#include "driver.h"

char *buffer;
char kb[16];
sceUsbdLddOps usbmouse_ops = {.name = "usbmouse",
                              .connect = usbmouse_attach,
                              .disconnect = usbmouse_detach,
                              .probe = usbmouse_probe};

static void *get_data(int fno, void *buffer, int len) {
    memcpy(buffer, kb, 0x10u);
    return buffer;
}

static void sifrpc_thread() {
    int thid;               // $v0
    SifRpcDataQueue_t qd;   // [sp+20h] [+20h] BYREF
    SifRpcServerData_t *v2; // [sp+38h] [+38h] BYREF
    sceSifInitRpc(0);
    thid = GetThreadId();
    sceSifSetRpcQueue(&qd, thid);
    sceSifRegisterRpc(v2, -2147483120, get_data, &buffer, 0, 0, &qd);
    sceSifRpcLoop(&qd);
}

/** Entry point. */
int start() {
    int gp0;             // $gp
    iop_thread_t thread; // [sp+10h] [+10h] BYREF
    int registerLddRet;  // [sp+28h] [+28h]
    int thid;            // [sp+2Ch] [+2Ch]

    registerLddRet = sceUsbdRegisterLdd(&usbmouse_ops);
    if (registerLddRet)
        printf("usb : sceUsbdRegisterLdd -> 0x%x\n", registerLddRet);
    memset(kb, 0, 16);
    thread.attr = 0x2000000;
    thread.thread = sifrpc_thread;
    thread.priority = 0x15;
    thread.stacksize = 0x1000;
    thread.option = 0;
    thid = CreateThread(&thread);
    if (thid <= 0)
        return 1;
    StartThread(thid, 0);
    return 0;
}
