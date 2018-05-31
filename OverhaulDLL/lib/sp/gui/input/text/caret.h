/*
    CONTRIBUTORS:
        Sean Pesce

*/
#pragma once

#ifndef SP_GUI_CARET_H_
#define SP_GUI_CARET_H_

#include "sp/sp.h"


__SP_NAMESPACE
namespace gui {
namespace in  {
namespace txt {


template <typename charT = char>
class caret
{
protected:
    charT _symbol  = (charT)'_';
    bool  _box     = false;
    bool  _visible = false;
    unsigned int _delay = 500; // Blink delay (milliseconds)

public:

    caret() {}

    caret(charT symbol) {
        _symbol = symbol;
    }

    caret(unsigned int delay) {
        _delay = delay;
        _visible = (_delay == 0) ? true : false;
    }

    caret(bool box_mode) {
        _box = box_mode;
    }

    caret(charT symbol, unsigned int delay) {
        _symbol = symbol;
        _delay  = delay;
        _visible = (_delay == 0) ? true : false;
    }

    caret(charT symbol, bool box_mode) {
        _symbol = symbol;
        _box    = box_mode;
    }

    caret(unsigned int delay, bool box_mode) {
        _delay = delay;
        _box   = box_mode;
        _visible = (_delay == 0) ? true : false;
    }

    caret(charT symbol, unsigned int delay, bool box_mode) {
        _symbol = symbol;
        _delay  = delay;
        _box    = box_mode;
        _visible = (_delay == 0) ? true : false;
    }

    ~caret() {}


    inline        charT symbol()   { return _symbol;  }
    inline         bool box_mode() { return _box;     }
    inline         bool showing()  { return _visible; }
    inline unsigned int delay()    { return _delay;   }

    inline void set_symbol(charT symbol)      { _symbol = symbol; }
    inline void set_box_mode(bool box_mode)   { _box = box_mode;  }
    inline void toggle_box_mode()             { _box = !_box;     }
    inline void blink() { _visible = !_visible; }
    inline void show()  { _visible = true;      }
    inline void hide()  { _visible = false;     }
    inline void set_delay(unsigned int delay) {
        _delay = delay;
        _visible = (_delay == 0) ? true : _visible;
    }
};


} // namespace txt
} // namespace in
} // namespace gui
__SP_NAMESPACE_CLOSE // namespace sp


#endif // SP_GUI_CARET_H_
