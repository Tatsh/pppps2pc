#include <usbd.h>

#include "shared.h"

char dword_2D18[2048];

int usbmouse_detach(int devId) {
    shared_t *p; // [sp+10h] [+10h]

    p = (shared_t *)sceUsbdGetPrivateData(devId);
    if (!p)
        return -1;
    if (p->port > 0)
        memset(&dword_2D18[2 * p->port], 0, 8u);
    unit_free(p);
    printf("usbmouse%d: detached : port %d\n", p->port1, p->port);
    return 0;
}
