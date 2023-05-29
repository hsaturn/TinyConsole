#include "TinyTerm.h"
#include "TinyStreaming.h"

#define TINY_CONSOLE_DEBUG 0

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
TinyTerm Term(Serial);
#endif

static const char* CSI="\033[";
static const char ESC='\033';
static const char BEL='\007';

TinyTerm::TinyTerm(Stream& ref_stream)
  : stream(&ref_stream)
{
}

void TinyTerm::begin(Stream& ser)
{
  stream = &ser;
  delay(100);
  is_term = false;
  stream = &ser;
  getTermSize();
  if (is_term) getTermSize();
}

void TinyTerm::clear()
{
  if (is_term) *stream << CSI << "2J";
  gotoxy(0,0);
}

void TinyTerm::getTermSize()
{
  csi.clear();
  sx=0;
  sy=0;
  csi6n = true;
  gotoxy(255,255);
  *stream << CSI << "6n";
  delay(100);
  loop();
  if (is_term) return;
  sx=0;
  sy=0;
}

char TinyTerm::waitChar()
{
  for(int i=0; i<30; i++)
  {
    if (stream->available())
      return stream->read();
    delay(1);
  }
  return 0;
}

void TinyTerm::mouseTrack(bool on)
{
  *stream << CSI << (on ? "?1000h" : "?1000l");
}


const TinyTerm& TinyTerm::eraseEol() const
{
  if (is_term) *stream << CSI << 'K';
  return *this;
}

const TinyTerm& TinyTerm::saveCursor() const
{
  if (is_term) *stream << CSI << 's';
  return *this;
}

const TinyTerm& TinyTerm::cursorVisible(bool visible) const
{
  if (is_term) *stream << CSI << "?25" << (visible ? 'h' : 'l');
  return *this;
}

const TinyTerm& TinyTerm::restoreCursor() const
{
  if (is_term) *stream << CSI << 'u';
  return *this;
}

const TinyTerm& TinyTerm::gotoxy(unsigned char row, unsigned char col) const
{
  if (is_term) *stream << CSI << (int) row << ';' << (int) col << 'H';
  return *this;
}

const TinyTerm& TinyTerm::fg(enum TinyTerm::Color c) const
{
  if (is_term) *stream << CSI << static_cast<int>(c) << 'm';
  return *this;
}

const TinyTerm& TinyTerm::reset() const
{
  if (is_term) *stream << "\033c";
  return *this;
}

void TinyTerm::loop()
{
  if (stream->available())
  {
    char c=stream->read();
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
  if (is_term) *stream << CSI << "2J";
  return *this;
}

const TinyTerm& TinyTerm::title(const char* title) const
{
  if (is_term) *stream << '\033' << "]2;" << title << BEL;
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
      sy=0;
      sx=255;
      while ((e>='0' and e<='9') or e==';' or e=='R')
      {
        csi += e;
        if (e=='R') return;
        if (e==';')
        {
          sx=0;
        }
        else if (sx==255)
          sy=10*sy+e-'0';
        else
          sx=10*sx+e-'0';
        e=waitChar();
      }
      return;
    }
    else if (e == 77) // mouse
    {
      static MouseEvent event;
      char f=waitChar();
      event.value = f;
      event.x = waitChar() - '!';
      event.y = waitChar() - '!';
      if (callback_mouse) callback_mouse(event);
    }
    else if (callback_key)
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
            if (f=='4') callback_key(KEY_F12);
						if (f=='5') callback_key(KEY_F5);
						if (f>='7' and f<='9') callback_key(KeyCode(KEY_F6+f-'7'));
						if (f>='0' and f<='1') callback_key(KeyCode(KEY_F9+f-'0'));
					}
					break;
				}
        case 5: callback_key(KeyCode(KEY_PGUP)); break;
        case 6: callback_key(KeyCode(KEY_PGDOWN)); break;
        default:
          Term << __LINE__ << " unhandled " << e << endl;
			}
		}

#if TINY_CONSOLE_DEBUG
    static int counter=0;
    saveCursor();
    gotoxy(11,1);
    (*this).bg(white).fg(black);
    (*this) << "code esc(d=" << (int)d << ", e=" << (int)e
      << ", '" << (char)e << "')" << ',' << counter++;
    restoreCursor();
#endif

  }
  else if (d=='O' and e>='P' and e<='S' and callback_key)
  {
    Term << __LINE__ << " e=" << e << endl;
    callback_key(KeyCode(KEY_F1+e-'P'));
  }
  else if (d==0 and callback_key)
  {
    callback_key(KeyCode(KEY_ESC));
  }
}
