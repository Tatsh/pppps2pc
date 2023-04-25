#ifndef DRIVER_H
#define DRIVER_H

#include <usbd.h>

#include "callbacks.h"
#include "shared.h"
#include "utils.h"

static const int USB_ID = 0x110;
static const int USB_MAX_POWER = 0xC8;
static const int USB_DESCRIPTOR_TYPE = 0x21;
static const int USB_DEVICE_CLASS = 0x0;
static const int USB_DEVICE_PROTOCOL = 0x22;
static const int USB_DEVICE_PROTOCOL_LENGTH = 0x09;
static const int USB_DEVICE_PROTOCOL_MAX_SIZE = 0x52;
static const int USB_PROTOCOL_TOTAL_LENGTH = 0x2200;
static const int USB_DEVICE_VENDOR = 0x5051;
static const int USB_DEVICE_ENDPOINT_ADDRESS = 0x81;
static const unsigned int USB_PORT_A = 0x02u;
static const unsigned int USB_PORT_B = 0x01u;
static const int USB_MAX_PACKET_LO_BYTE = 0x08;
static const int USB_MAX_PACKET_HI_BYTE = 0x01;

int usbmouse_attach(int devId);
int usbmouse_detach(int devId);
int usbmouse_probe(int devId);

#endif // DRIVER_H
