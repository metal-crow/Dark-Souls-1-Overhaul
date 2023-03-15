#include "MainLoop.h"
#include "DarkSoulsOverhaulMod.h"
#include "SP/memory/injection/asm/x64.h"
#include <vector>
#include <tuple>
#include <mutex>

extern "C" {
    uint64_t main_game_loop_injection_return;
    void main_game_loop_injection();
    void main_game_loop_injection_helper();
}

void MainLoop::start()
{
    ConsoleWrite("Starting main loop");

    uint8_t *write_address = (uint8_t*)(MainLoop::main_game_loop_injection_offset + Game::ds1_base);
    sp::mem::code::x64::inject_jmp_14b(write_address, &main_game_loop_injection_return, 2, &main_game_loop_injection);
}

//contains the function ptr and the data
static std::vector<std::tuple<MainLoopCallback, void*, std::string>> callbacks;

//adding an item from a std::vector invalidates existing iterators, so we need to only add new items when we're not iterating
static std::vector<std::tuple<MainLoopCallback, void*, std::string>> callbacks_queue;
std::mutex callbacks_queue_mtx;

//given the function and the data arg for it, save it so we call it every frame until it returns false
void MainLoop::setup_mainloop_callback(MainLoopCallback func, void* data, std::string name)
{
    callbacks_queue_mtx.lock();
    callbacks_queue.push_back(std::make_tuple(func, data, name));
    callbacks_queue_mtx.unlock();
}

static const uint64_t CALLBACK_FUNC_TIME_WARNING = (uint64_t)(0.1 * 10000000); //100 ms

void main_game_loop_injection_helper()
{
    //move any items from the queue to our real list
    callbacks_queue_mtx.lock();
    for (int i = 0; i < callbacks_queue.size(); i++)
    {
        callbacks.push_back(callbacks_queue[i]);
    }
    callbacks_queue.clear();
    callbacks_queue_mtx.unlock();

    //run through all the callbacks
    auto iter = callbacks.begin();
    while (iter != callbacks.end())
    {
        auto callback = *iter;
        MainLoopCallback func = std::get<0>(callback);
        void* data = std::get<1>(callback);

        //monitor the time for these callbacks to isolate issues
        uint64_t start_time = Game::get_accurate_time();
        bool result = func(data);
        uint64_t end_time = Game::get_accurate_time();
        uint64_t length_time = end_time - start_time;
        if (length_time > CALLBACK_FUNC_TIME_WARNING)
        {
            ConsoleWrite("WARNING: Callback function %s took %f ms", std::get<2>(callback).c_str(), length_time/10000.0f);
        }

        //when the function returns false, remove from our callbacks
        if (result == false)
        {
            //removing an item invalidates existing iterators, so we need to get a new one
            iter = callbacks.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}
