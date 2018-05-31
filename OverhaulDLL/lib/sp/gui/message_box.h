/*
    CONTRIBUTORS:
        Sean Pesce

*/
#pragma once

#ifndef SP_GUI_MESSAGE_BOX_H_
#define SP_GUI_MESSAGE_BOX_H_

#include "sp/sp.h"
#include "sp/log.h"
#include <string>


#ifdef    _WIN32
#define SP_MSG_BOX_ERROR MB_OK
#define SP_MSG_BOX_WARN  MB_OK
#define SP_MSG_BOX_INFO  MB_OK
#else  // UNIX
#include "SDL.h"
#include "SDL_messagebox.h"
#define SP_MSG_BOX_ERROR SDL_MESSAGEBOX_ERROR
#define SP_MSG_BOX_WARN  SDL_MESSAGEBOX_WARNING
#define SP_MSG_BOX_INFO  SDL_MESSAGEBOX_INFORMATION
#endif // _WIN32


__SP_NAMESPACE
namespace gui {


template <typename charT>
int open_dialog(const std::basic_string<charT> &title, const std::basic_string<charT> &message, uint32_t flags = SP_MSG_BOX_INFO)
{
    int result = 0;
#ifdef    _WIN32
    if (sizeof(charT) == 1) {
        result = MessageBoxA(NULL, (const char*)message.c_str(), (const char*)title.c_str(), flags);
    } else {
        result = MessageBoxW(NULL, (const wchar_t*)message.c_str(), (const wchar_t*)title.c_str(), flags);
    }
    if (!result) {
        // Failed to display message box
        SP_LOG("Error Presenting MessageBox (Code %d)\n", result);
    }
#else  // UNIX
    // @TODO: This is untested code. Maybe use FLTK2.0 or Qt instead?
    if (SDL_Init(0) < 0) {
        SP_LOG("Couldn't initialize SDL: %s\n", SDL_GetError());
        return 0;
    }
    result = SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), NULL);
    if (result == -1) {
        SP_LOG("Error Presenting MessageBox: %s\n", SDL_GetError());
    }
#endif // _WIN32
    return result;
}


} // namespace gui
__SP_NAMESPACE_CLOSE // namespace sp


#endif // SP_GUI_MESSAGE_BOX_H_
