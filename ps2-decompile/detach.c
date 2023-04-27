#include <usbd.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "shared.h"

static char dword_2D18[2048];

int usbmouse_detach(int devId) {
    shared_t *p;

    if ((p = sceUsbdGetPrivateData(devId)) == NULL)
        return -1;

    if (p->port > 0)
        memset(&dword_2D18[2 * p->port], 0, 8u);

    unit_free(p);

#ifdef DEBUG
    (void) printf("usbmouse%d: detached : port %d\n", p->port1, p->port);
#endif

    return 0;
}
