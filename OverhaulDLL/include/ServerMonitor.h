#pragma once
#ifndef SERVERMONITOR_H
#define SERVERMONITOR_H

#include <cstdint>
#include <unordered_set>
#include <mutex>

class ServerMonitor
{
public:
    static void start();
    static void clearBreakinList();
    static bool checkIfUserIsBreakin(uint64_t steamid);
    static std::mutex BreakinUsers_Mtx;
    static std::unordered_set<uint64_t> BreakinUsers;
private:
    static const uint64_t BreakIn_UserSteamId_offset = 0xbd0efb;
};



#endif
