#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include <usbd.h>

#include "callbacks.h"
#include "shared.h"

int data_transfer(shared_t *g);

#ifdef DEBUG
UsbDeviceDescriptor *dump_desc(int devId);
#endif
#endif // UTILS_H
