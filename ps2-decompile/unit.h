#ifndef UNIT_H
#define UNIT_H

#include "shared.h"
#include <types.h>

shared_t *unit_alloc(const int id, const int maxPacketSize, const u8 interfaceNumber, const u8 alternateSetting);
void unit_free(shared_t *p);

#endif // UNIT_H
