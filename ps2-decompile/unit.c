#include "unit.h"

shared_t *unit_alloc(const int id, const int maxPacketSize, const u8 interfaceNumber, const u8 alternateSetting) {
    // int v4;        // $gp
    // int v5;        // $v1

    shared_t *mem; // [sp+10h] [+10h]

    if ((mem = AllocSysMemory(0, maxPacketSize + 32820, 0)) == NULL)
        return NULL;

    memset(mem, 0, sizeof(mem));

    mem->devId = id;
    //v5 = *(_DWORD *)(v4 + 0xFFFF8064); // 0x2D20 bss kb
    mem->port1 = 0; // v5;
    //*(_DWORD *)(v4 + 0xFFFF8064) = v5 + 1;
    mem->maxPacketSize = maxPacketSize;
    mem->interfaceNumber = interfaceNumber;
    mem->alternateSetting = alternateSetting;
    mem->unk5 = 1;
    return mem;
}

void unit_free(shared_t *p) {
    //int v1;                     // $gp
    //--*(_DWORD *)(v1 - 0x7F9C); // 0x2D20 bs kb
    FreeSysMemory(p);
}
