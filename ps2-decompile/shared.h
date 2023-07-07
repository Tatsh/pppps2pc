#ifndef SHARED_H
#define SHARED_H

typedef struct t_shared {
    int devId;
    int port1;
    int configEndpoint;
    int dataEndpoint;
    int maxPacketSize;
    int interfaceNumber;
    int alternateSetting;
    int count;
    int port;
    int unk5;
    int descriptorChecksPassed;
    int unk7;
    char buf[2048];
    char data[30732];
} shared_t;

#endif // SHARED_H
