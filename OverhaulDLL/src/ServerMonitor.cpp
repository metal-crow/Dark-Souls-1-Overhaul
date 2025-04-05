#include "ServerMonitor.h"
#include "GameData.h"
#include "SP/memory/injection/asm/x64.h"
#include <cstdlib>

extern "C" {
    uint64_t BreakIn_UserSteamId_injection_return;
    void BreakIn_UserSteamId_injection();
    void BreakIn_UserSteamId_injection_helper(char* steamid);
}

std::mutex ServerMonitor::BreakinUsers_Mtx;
std::unordered_set<uint64_t> ServerMonitor::BreakinUsers;

void ServerMonitor::start()
{
    ConsoleWrite("Monitoring server packets...");

    uint64_t write_address = Game::ds1_base + BreakIn_UserSteamId_offset;
    sp::mem::code::x64::inject_jmp_14b((void*)write_address, &BreakIn_UserSteamId_injection_return, 2, &BreakIn_UserSteamId_injection);
}

void ServerMonitor::clearBreakinList()
{
    ServerMonitor::BreakinUsers_Mtx.lock();
    BreakinUsers.clear();
    ServerMonitor::BreakinUsers_Mtx.unlock();
}

bool ServerMonitor::checkIfUserIsBreakin(uint64_t steamid)
{
    ServerMonitor::BreakinUsers_Mtx.lock();
    bool res = BreakinUsers.contains(steamid);
    ServerMonitor::BreakinUsers_Mtx.unlock();
    return res;
}

void BreakIn_UserSteamId_injection_helper(char* steamid)
{
    char* end = nullptr;
    uint64_t id = strtoull(steamid, &end, 16);
    if (id == 0 || errno == ERANGE)
    {
        FATALERROR("Unable to convert breakin user id %s to int", steamid);
    }
    ConsoleWrite("Got break in user %llx", id);
    ServerMonitor::BreakinUsers_Mtx.lock();
    ServerMonitor::BreakinUsers.insert(id);
    ServerMonitor::BreakinUsers_Mtx.unlock();
}
