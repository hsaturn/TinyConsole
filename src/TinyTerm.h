// vim: ts=2 sw=2 expandtab
#pragma once
#include "TinyStreaming.h"
#include <deque>
#include <functional>
#include <Stream.h>
#define TINY_term_AUTOSIZE 0

class TinyTerm
{
  public:

    enum Color
    {
      black = 30,
      red = 31,
      green = 32,
      yellow = 33,
      blue = 34,
      magenta = 35,
      cyan = 36,
      white = 37,
      gray = 90,
      bright_red = 91,
      bright_green = 92,
      bright_yellow = 93,
      bright_blue = 94,
      bright_magenta = 95,
      bright_cyan = 96,
      bright_white = 97,
      none = 255
    };

    enum KeyCode
    {
      KEY_SUPPR = 0x100, KEY_BACK,
      KEY_RETURN,
      KEY_HOME, KEY_END,
      KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
      KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
      KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12
    };

    enum Operation
    {
      hide_cur,
      show_cur,
      erase_to_end,
      save_cursor,
      restore_cursor
    };

    using CallBackKey = void(*)(int fkey);

    TinyTerm();
    void begin(long baud);  // Init with Serial
    void begin(Stream&);
    void loop();

    void onKey(std::function<void(KeyCode)> cb) { callback_key = cb; }

    const TinyTerm& gotoxy(unsigned char x, unsigned char y) const;
    const TinyTerm& cursorVisible(bool visible) const;

    template<class Type>
    void printAt(unsigned char row, unsigned char col, const Type &what) const
    {
      saveCursor();
      cursorVisible(false);
      gotoxy(row, col);
      *serial << what;
      restoreCursor();
      cursorVisible(true);
    }

    const TinyTerm& cls() const;
    const TinyTerm& title(const char*) const;
    const TinyTerm& reset() const;
    const TinyTerm& saveCursor() const;
    const TinyTerm& restoreCursor() const;
    const TinyTerm& eraseEol() const;
    const TinyTerm& fg(enum Color c) const;
    const TinyTerm& bg(enum Color c) const { return fg(static_cast<enum Color>(static_cast<int>(c)+10)); }

    template<class Type>
    friend TinyTerm& operator << (TinyTerm& term, Type value)
    {
      *term.serial << value;
      return term;
    }

    friend TinyTerm& operator << (TinyTerm& term, Color color)
    {
      term.fg(color);
      return term;
    }

    friend TinyTerm& operator << (TinyTerm& term, Operation op)
    {
      if (op == erase_to_end)
        term.eraseEol();
      else if (op == save_cursor)
        term.saveCursor();
      else if (op == restore_cursor)
        term.restoreCursor();
      else
        term.cursorVisible(op == show_cur);
      return term;
    }

    bool isTerm() const { return is_term; }
    void clear();

  private:
    char waitChar();
    void handleEscape();
    std::function<void(KeyCode)> callback_key;
    Stream* serial = nullptr;
    bool is_term = false;
    bool csi6n = false;

    bool getTermSize();
#if TINY_term_AUTOSIZE
  public:
    unsigned char sx;
    unsigned char sy;
#endif
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
extern TinyTerm Term;
#endif
