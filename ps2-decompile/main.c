#include <sifrpc.h>
#include <thbase.h>
#include <usbd.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "driver.h"
#include "globals.h"

char *buffer;
char kb[16];

static sceUsbdLddOps usbmouse_ops = {
    .name = "usbmouse",
    .connect = usbmouse_attach,
    .disconnect = usbmouse_detach,
    .probe = usbmouse_probe
};

static void *get_data(int fno, void *buffer, int len) {
    (void) fno;
    (void) len;

    memcpy(buffer, kb, THREAD_KB_SIZE);
    return buffer;
}

static void sifrpc_thread(void) {
    int thid;
    SifRpcDataQueue_t qd;
    SifRpcServerData_t *sd;
    sceSifInitRpc(0);
    thid = GetThreadId();
    sceSifSetRpcQueue(&qd, thid);
    sceSifRegisterRpc(sd, THREAD_RPC, get_data, &buffer, 0, 0, &qd);
    sceSifRpcLoop(&qd);
}

/** Entry point. */
int start(void) {
    iop_thread_t thread;
    int registerLddRet, thid;

    if ((registerLddRet = sceUsbdRegisterLdd(&usbmouse_ops))) {
#ifdef DEBUG
        (void) printf("usb : sceUsbdRegisterLdd -> 0x%x\n", registerLddRet);
#endif
    }

    memset(kb, 0, THREAD_KB_SIZE);
    thread.attr = THREAD_ATTRIBUTE;
    thread.thread = sifrpc_thread;
    thread.priority = THREAD_PRIORITY;
    thread.stacksize = THREAD_STACK_SIZE;
    thread.option = 0;

    if ((thid = CreateThread(&thread)) <= 0)
        return 1;

    StartThread(thid, 0);
    return 0;
}
