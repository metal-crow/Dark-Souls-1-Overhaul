/*
    DARK SOULS OVERHAUL

    Contributors to this file:
        Sean Pesce  -  Reverse engineering, C++
*/

#include "DllMain.h"
#include "Menu/Dialog.h"


namespace Menu {
namespace Dlg  {

struct async_dlg_data {
    dlg_type dialog_type;
    unsigned long duration;
};

// Shows a dialog box for the specified duration (in milliseconds), then closes it
DWORD WINAPI _show_async_internal(LPVOID lpParam)
{
    if (lpParam)
    {
        async_dlg_data* params = reinterpret_cast<async_dlg_data*>(lpParam);
        dlg_type dialog_type = params->dialog_type;
        unsigned long duration = params->duration;
        delete params;
        params = NULL;

        uint32_t tmp_msg_id = Menu::Dlg::msg_id;
        show(dialog_type);
        Sleep(duration);
        if (flag(96) == static_cast<uint32_t>(dialog_type) && Menu::Dlg::msg_id == tmp_msg_id)
        {
            hide();
        }
    }
    return 0;
}

void show_async(dlg_type dialog_type, unsigned long duration)
{
    if (!CreateThread(NULL, 0, _show_async_internal, new async_dlg_data{ dialog_type, duration }, 0, NULL))
    {
        // Error creating new thread
    }
}

} // namespace Dlg
} // namespace Menu

