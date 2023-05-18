#include "TinyTerm.h"
#include "TinyStreaming.h"

#define TINY_CONSOLE_DEBUG 0

static const char* CSI="\033[";
static const char ESC='\033';
static const char BEL='\007';

TinyTerm::TinyTerm()
  : serial(&Serial)
{
}

void TinyTerm::begin(long baud)
{
	serial = &Serial;
	Serial.begin(baud);
}

void TinyTerm::begin(Stream& ser)
{
  is_term = false;
  serial = &ser;
  getTermSize();
}

bool TinyTerm::getTermSize()
{
  csi6n = true;
  *serial << CSI << "6n";
  delay(50);
  loop();
  if (not is_term) return false;

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
  *serial << "got sx=" << sx << ", sy=" << sy << ", dxdy=" << dx << ' ' << dy << endl;
  if (sx) sx--;
  if (sy) sy--;
  return true;
#else
  return false;
#endif
}

char TinyTerm::waitChar()
{
  for(int i=0; i<30; i++)
  {
    if (serial->available())
      return serial->read();
    delay(1);
  }
  return 0;
}

const TinyTerm& TinyTerm::eraseEol() const
{
  if (is_term) *serial << CSI << 'K';
  return *this;
}

const TinyTerm& TinyTerm::saveCursor() const
{
  if (is_term) *serial << CSI << 's';
  return *this;
}

const TinyTerm& TinyTerm::cursorVisible(bool visible) const
{
  if (is_term) *serial << CSI << "?25" << (visible ? 'h' : 'l');
  return *this;
}

const TinyTerm& TinyTerm::restoreCursor() const
{
  if (is_term) *serial << CSI << 'u';
  return *this;
}

const TinyTerm& TinyTerm::gotoxy(unsigned char row, unsigned char col) const
{
  if (is_term) *serial << CSI << (int) row << ';' << (int) col << 'H';
  return *this;
}

const TinyTerm& TinyTerm::fg(enum TinyTerm::Color c) const
{
  if (is_term) *serial << CSI << static_cast<int>(c) << 'm';
  return *this;
}

const TinyTerm& TinyTerm::reset() const
{
  if (is_term) *serial << "\033c";
  return *this;
}

void TinyTerm::loop()
{
  if (serial->available())
  {
    char c=serial->read();
		if (c==27)
      handleEscape();
		else if (callback_key)
		{
			if (c==10 or c==13)
				callback_key(KEY_RETURN);
    	else if (c==8 or c==127)
    		callback_key(KEY_BACK);
			else if (c==126)
				callback_key(KEY_SUPPR);
      else
        callback_key(KeyCode(c));
		}

#if TINY_CONSOLE_DEBUG
    static int counter=0;
    saveCursor();
    gotoxy(10,1);
    (*this).bg(white).fg(black);
    (*this) << "code(" << (int)c << ',' << counter++ << ')';
    restoreCursor();
#endif
  }
}

const TinyTerm& TinyTerm::cls() const
{
  if (is_term) *serial << CSI << "2J";
  return *this;
}

const TinyTerm& TinyTerm::title(const char* title) const
{
  if (is_term) *serial << '\033' << "]2;" << title << BEL;
  return *this;
}

void TinyTerm::handleEscape()
{
  char d=waitChar();
  char e=waitChar();
  if (d==91) // [
  {
    if (csi6n and (e>='0' and e<='9'))  // Size report
    {
      csi6n = false;
      is_term = true;
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
    if (callback_key)
		{
			switch(e)
			{
				case  51: break; // supr (but handled by 126)
				case  65: callback_key(KEY_UP); break;
				case  66: callback_key(KEY_DOWN); break;
				case  67: callback_key(KEY_RIGHT); break;
				case  68: callback_key(KEY_LEFT); break;
				case 'H': callback_key(KEY_HOME); break;
				case 'F': callback_key(KEY_END); break;
				case '1':
				case '2':
				{
					char f=waitChar();
					char g=waitChar();
					if (g=='~')
					{
						if (f=='5') callback_key(KEY_F5);
						if (f>='7' and f<='9') callback_key(KeyCode(KEY_F1-1+f-'7'+6));
						if (f>='0' and f<='4') callback_key(KeyCode(KEY_F1-1+f-'0'+9));
					}
					break;
				}
			}
		}

#if TINY_CONSOLE_DEBUG
    static int counter=0;
    saveCursor();
    gotoxy(11,1);
    (*this).bg(white).fg(black);
    (*this) << "code esc(d=" << (int)d << ", e=" << (int)e
      << ", '" << (char)e << "')" << ',' << counter++
    restoreCursor();
#endif

  }
  else if (d=='O' and e>='P' and e<='S' and callback_key)
  {
    callback_key(KeyCode(e-'P'+1));
  }
  else // if (histo_n) TODO was histo reset
  {
    saveCursor();
    (*this) << "histo reset if histo_n";
    restoreCursor();
  	/*
    histo_n = false;
    if (input.size()>cursor)
      input.erase(cursor);
      */
  }
}
