#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <usbd.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "globals.h"
#include "shared.h"

static UsbDeviceRequest REQ_GET_REPORT = {
    .requesttype = USB_DIR_IN,
    .request = USB_REQ_GET_DESCRIPTOR,
    .value = 768,
    .index = 0,
    .length = 0x800
};

static UsbDeviceRequest REQ_SET_CONFIG_DONE = {
    .requesttype = USB_DIR_OUT | USB_RECIP_DEVICE,
    .request = USB_REQ_GET_DESCRIPTOR,
    .value = 0x2200,
    .index = 0,
    .length = 0x400
};

int data_transfer_done(int result, int count, shared_t *g);
int get_report_done(int result, int count, shared_t *g);
int get_string_done(int result, int count, shared_t *g);
int led_transfer_done(int result, int count, shared_t *g);
int set_config_done(int result, int count, shared_t *g);
int set_interface_done(int result, int count, shared_t *g);

#endif // CALLBACKS_H
