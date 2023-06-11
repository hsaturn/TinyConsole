// vim: ts=2 sw=2 expandtab
#pragma once
#include "TinyStreaming.h"
#include <deque>
#include <functional>
#include <utility>
#include <Stream.h>

#if ESP32
// Not available everywhere ...
template <typename T, typename U = T>
T exchange(T& obj, U&& new_value)
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
};
#endif

class TinyTerm : public Stream
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

    struct MouseEvent
    {
      union
      {
        struct {
          uint8_t button: 2;  // 0:left 1:mid 2:right 3:none
          uint8_t shift: 1;
          uint8_t unused_1: 1;
          uint8_t ctrl: 1;
          uint8_t unuse_2: 1;
          uint8_t wheel: 1;
        } event;
        uint8_t value;
      };
      unsigned x;
      unsigned y;
    };

    enum KeyCode
    {
      KEY_CTRL_A = 1, KEY_CTRL_B, KEY_CTRL_C, KEY_CTRL_D, KEY_CTRL_E, KEY_CTRL_F,
      KEY_CTRL_G, KEY_CTRL_H, KEY_CTRL_I, KEY_CTRL_J, KEY_CTRL_K, KEY_CTRL_L,
      KEY_CTRL_M, KEY_CTRL_N, KEY_CTRL_O, KEY_CTRL_P, KEY_CTRL_Q, KEY_CTRL_R,
      KEY_CTRL_S, KEY_CTRL_T, KEY_CTRL_U, KEY_CTRL_V, KEY_CTRL_W, KEY_CTRL_X,
      KEY_CTRL_Y, KEY_CTRL_Z,
      KEY_SUPPR = 0x100, KEY_BACK,
      KEY_RETURN,
      KEY_HOME, KEY_END, KEY_ESC, KEY_INS,
      KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
      KEY_PGUP, KEY_PGDOWN,
      KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
      KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    };

    enum Operation
    {
      hide_cur,
      show_cur,
      erase_to_end,
      save_cursor,
      restore_cursor
    };

    using CallBackKey = std::function<void(KeyCode)>;
    using CallBackMouse = std::function<void(const MouseEvent&)>;

    TinyTerm(Stream& pstream);
    void begin(Stream& pstream);  // FIXME ambiguous with ctor
    void loop();

    CallBackKey onKey(CallBackKey cb) { return exchange(callback_key, cb); }
    CallBackMouse onMouse(CallBackMouse cb) { mouseTrack(true); return exchange(callback_mouse, cb); }

    const TinyTerm& gotoxy(unsigned char x, unsigned char y) const;
    const TinyTerm& cursorVisible(bool visible) const;

    template<class Type>
    void printAt(unsigned char row, unsigned char col, const Type &what) const
    {
      saveCursor();
      cursorVisible(false);
      gotoxy(row, col);
      *stream << what;
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

    int available() override { return stream->available(); }
    int read() override { return stream->read(); }
    int peek() override { return stream->peek(); }
    size_t write(uint8_t t) override { return stream->write(t); }

    template<class Type>
    friend TinyTerm& operator << (TinyTerm& term, Type value)
    { *term.stream << value; return term; }

    friend TinyTerm& operator << (TinyTerm& term, const char* ptr)
    { *term.stream << ptr; return term; }

    template<class Type>
    friend TinyTerm& operator << (TinyTerm& term, Type* ptr)
    { *term.stream << (long)ptr; return term; }

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
    void getTermSize();
    void mouseTrack(bool on);
    void clear();

  private:
    char waitChar();
    void handleEscape();
    CallBackKey callback_key;
    CallBackMouse callback_mouse;
    Stream* stream = nullptr;
    bool is_term = false;
    bool csi6n = false;

  public:
    unsigned char sx; // cols
    unsigned char sy; // rows
    std::string csi;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
extern TinyTerm Term;
#endif
