#include "TinyConsole.h"
#include "TinyStreaming.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
TinyConsole Console;
#endif

#define TINY_CONSOLE_DEBUG 0

static const char* CSI="\033[";
static const char ESC='\033';
static const char BEL='\007';

TinyConsole::TinyConsole()
  : ps1("> "), serial(&Serial)
{
}

void TinyConsole::begin(long baud)
{
	serial = &Serial;
	Serial.begin(baud);
}

void TinyConsole::begin(Stream& ser)
{
  term = false;
  serial = &ser;
  getTermSize();
}

bool TinyConsole::getTermSize()
{
  csi6n = true;
  *serial << CSI << "6n";
  delay(50);
  loop();
  if (not term) return false;

#if TINY_CONSOLE_AUTOSIZE
  auto end = millis()+900;  // Avoid WDT
  unsigned char dx=64;
  unsigned char dy=64;
  unsigned char tx=128;
  unsigned char ty=128;
  *serial << endl << endl;
  *serial << "millis=" << millis() << endl;
  while ((dx or dy) and millis()<end)
  {
    delay(50);
    sx=0;
    sy=0;
    *serial << "try " << tx << "x" << ty << endl;
    saveCursor();
    gotoxy(tx,ty);
    // Detect if it is a terminal (or the IDE's serial monitor)
    // also try to compute size of terminal
    *serial << CSI << "6n";
    delay(10);
    loop();
    if (serial->available()) loop();
    if (sx==tx)
      tx+=dx;
    else
      tx-=dx;
    if (sy==ty)
      ty+=dy;
    else
      ty-=dy;

    dx >>=1;
    dy >>=1;

  }
  *serial << endl << endl;
  *serial << "got sx=" << sx << ", sy=" << sy << endl;
  if (sx) sx--;
  if (sy) sy--;
  return true;
#else
  return false;
#endif
}

char TinyConsole::waitChar()
{
  for(int i=0; i<30; i++)
  {
    if (serial->available())
      return serial->read();
    delay(1);
  }
  return 0;
}

const TinyConsole& TinyConsole::eraseEol() const
{
  if (term) *serial << CSI << 'K';
  return *this;
}

const TinyConsole& TinyConsole::saveCursor() const
{
  if (term) *serial << CSI << 's';
  return *this;
}

const TinyConsole& TinyConsole::cursorVisible(bool visible) const
{
  if (term) *serial << CSI << "?25" << (visible ? 'h' : 'l');
  return *this;
}

const TinyConsole& TinyConsole::restoreCursor() const
{
  if (term) *serial << CSI << 'u';
  return *this;
}

const TinyConsole& TinyConsole::gotoxy(unsigned char row, unsigned char col) const
{
  if (term) *serial << CSI << (int) row << ';' << (int) col << 'H';
  return *this;
}

const TinyConsole& TinyConsole::fg(enum TinyConsole::Color c) const
{
  if (term) *serial << CSI << static_cast<int>(c) << 'm';
  return *this;
}

const TinyConsole& TinyConsole::prompt() const
{
  if (term)
    *serial << CSI << 'G' << ps1 << input << CSI << "0K" << CSI << (int)(ps1.size()+cursor+1) << 'G';
  return *this;
}

const TinyConsole& TinyConsole::reset() const
{
  if (term) *serial << "\033c";
  return *this;
}

void TinyConsole::loop()
{
  if (serial->available())
  {
    char c=serial->read();
    if (c==27)
      handleEscape();
    else if (c==8 or c==127)
    {
      if (input.size() and cursor>0)
      {
        input.erase(--cursor, 1);
      }
    }
    else if (c==126)
    {
      if (cursor < input.size())
        input.erase(cursor, 1);
    }
    else if (c==10 or c==13)
    {
      if (histo_n)
      {
        if (history.size()>1)
        {
          auto it=history.begin()+histo_n-1;
          string s=*it;
          history.erase(it);
          history.push_front(s);
        }
        histo_n = 0;
      }
      else if (input.size())
      {
        history.push_front(input);
      }
      if (history.size()>histo_max)
        history.pop_back();

      if (callback) callback(input);
      input.clear();
      cursor=0;
      prompt();
    }
    else if (c>=32)
    {
      input.insert(cursor++, 1, c);
    }

#if TINY_CONSOLE_DEBUG
    static int counter=0;
    saveCursor();
    gotoxy(10,1);
    (*this).bg(white).fg(black);
    (*this) << "code(" << (int)c << ',' << counter++ << ')';
    restoreCursor();
#endif

		prompt();
  }
}

const TinyConsole& TinyConsole::cls() const
{
  if (term) *serial << CSI << "2J";
  return *this;
}

const TinyConsole& TinyConsole::title(const char* title) const
{
  if (term) *serial << '\033' << "]2;" << title << BEL;
  return *this;
}

void TinyConsole::handleEscape()
{
  char d=waitChar();
  char e=waitChar();
  if (d==91) // [
  {
    if (csi6n and (e>='0' and e<='9'))  // Size report
    {
      csi6n = false;
      term = true;
#if TINY_CONSOLE_AUTOSIZE
      sx=0;
      sy=255;
      while ((e>='0' and e<='9') or e==';' or e=='R')
      {
        if (e=='R') return;
        if (e==';')
        {
          sy=0;
        }
        else if (sy==255)
          sx=10*sx+e-'0';
        else
          sy=10*sy+e-'0';
        e=waitChar();
      }
      *serial << endl;
      return;
#else
      while(waitChar());
#endif
    }
    if (e==51) // supr (but handled by 126)
    {
    }
    else if (e==65 or e==66) // cursor up / down
    {
      string start = input.substr(0, cursor);
      if (e==65) // up
      {
        while(histo_n < history.size())
        {
          auto it = history.begin() + histo_n;
          histo_n++;

          if (it->substr(0,cursor)==start)
          {
            input = *it;
            break;
          }
        }
      }
      else if (e==66 and histo_n) // down
      {
        histo_n--;
        if (histo_n)
        {
          while (histo_n)
          {
            auto it = history.begin() + histo_n - 1;
            if (it->substr(0,cursor)==start)
            {
              input = *it;
              break;
            }
            else
              histo_n--;
          }
        }
        if (histo_n == 0)
        {
          input.erase(cursor);
        }
      }
    }
    else if (e==67 and cursor < input.size())  // cursor right
    {
      cursor++;
    }
    else if (e==68 and cursor > 0) // cursor left
    {
      cursor--;
    }
    else if (e=='H')  // home
    {
      cursor=0;
    }
    else if (e=='F') // End
    {
      cursor=input.size();
    }
    else if ((e=='1' or e=='2') and callback_fn)
    {
      char f=waitChar();
      char g=waitChar();
      if (g=='~')
      {
        if (f=='5') callback_fn(5);
        if (f>='7' and f<='9') callback_fn(f-'7'+6);
        if (f>='0' and f<='4') callback_fn(f-'0'+9);
      }
    }

#if TINY_CONSOLE_DEBUG
    static int counter=0;
    saveCursor();
    gotoxy(11,1);
    (*this).bg(white).fg(black);
    (*this) << "code esc(d=" << (int)d << ", e=" << (int)e
      << ", '" << (char)e << "')" << ',' << counter++ << " h=" << histo_n << ' '
      << " cur=" << (int) cursor
      << " hsz=" << history.size();

    restoreCursor();
#endif

  }
  else if (d=='O' and e>='P' and e<='S' and callback_fn)
  {
    callback_fn(e-'P'+1);
  }
  else if (histo_n) // reset
  {
    histo_n = false;
    if (input.size()>cursor)
      input.erase(cursor);
  }
}
