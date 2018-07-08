#include "AntiAntiCheat.h"
#include "GameData.h"

void AntiAntiCheat::start() {
    //patch out the hash comparison check
    void* writeaddress = (void*)(Game::ds1_base + 0x3459259);
    uint8_t patch[] = { 0x39, 0xC0 };
    bool res = sp::mem::patch_bytes(writeaddress, patch, sizeof(patch));
    if (!res) FATALERROR("Unable to disable anti-cheat");
}
