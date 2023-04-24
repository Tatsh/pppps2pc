#ifndef SHARED_H
#define SHARED_H

typedef struct t_shared {
    const int devId;
    int port1;
    int configEndpoint;
    int dataEndpoint;
    const int maxPacketSize;
    const int interfaceNumber;
    const int alternateSetting;
    int count;
    int port;
    int unk5;
    int descriptorChecksPassed;
    int unk7;
    char buf[2048];
    char unk8[30732];
} shared_t;

#endif // SHARED_H
