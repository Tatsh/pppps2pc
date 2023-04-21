#include "unit.h"

shared_t *unit_alloc(int id, int maxPacketSize, u8 interfaceNumber, u8 alternateSetting) {
    // int v4;        // $gp
    // int v5;        // $v1
    shared_t *mem; // [sp+10h] [+10h]

    mem = AllocSysMemory(0, maxPacketSize + 32820, 0);
    if (mem) {
        mem->devId = id;
        //v5 = *(_DWORD *)(v4 + 0xFFFF8064); // 0x2D20 bss kb
        mem->port1 = 0; // v5;
        //*(_DWORD *)(v4 + 0xFFFF8064) = v5 + 1;
        mem->maxPacketSize = maxPacketSize;
        mem->interfaceNumber = interfaceNumber;
        mem->alternateSetting = alternateSetting;
        mem->count = 0;
        mem->port = 0;
        mem->unk5 = 1;
        mem->unk7 = 0;
        //*(int *)&mem->unk8[0x7800] = 0;
        mem->descriptorChecksPassed = 0;
        memset(mem->buf, 0, 0x800u);
    }
    return mem;
}

void unit_free(shared_t *p) {
    //int v1;                     // $gp

    //--*(_DWORD *)(v1 - 0x7F9C); // 0x2D20 bs kb
    FreeSysMemory(p);
}
