#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "shared.h"

int data_transfer_done(int result, int count, shared_t *g);
int set_config_done(int result, int count, shared_t *g);

#endif // CALLBACKS_H
