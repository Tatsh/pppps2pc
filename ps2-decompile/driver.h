#ifndef DRIVER_H
#define DRIVER_H

int usbmouse_attach(int devId);
int usbmouse_detach(int devId);
int usbmouse_probe(int devId);

#endif // DRIVER_H
