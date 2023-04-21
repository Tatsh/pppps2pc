#ifndef UNIT_H
#define UNIT_H

#include "shared.h"
#include <types.h>

shared_t *unit_alloc(int id, int maxPacketSize, u8 interfaceNumber, u8 alternateSetting);
void unit_free(shared_t *p);

#endif // UNIT_H
