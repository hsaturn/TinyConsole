#include "TinyConsole.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
TinyConsole Console(&Term);
#endif

TinyConsole::TinyConsole(TinyTerm* term)
	: ps1("> "), term(term)
{
	if (term)
	{
		term->onKey([this] (TinyTerm::KeyCode key) -> void
				{ this->onKey(key); });
	}
}

static const char* CSI="\033[";
void TinyConsole::prompt() const
{
	if (ps1.length() and term and term->isTerm())
	{
		// TODO should not use CSI sequences but term methods
    *term << CSI << 'G' << ps1 << input << CSI << "0K" << CSI << (int)(ps1.size()+cursor+1) << 'G';
	}
}


void TinyConsole::onKey(TinyTerm::KeyCode key)
{
  switch(key)
  {
    case TinyTerm::KEY_ESC:
    {
      if (histo_n)
        histo_n = false;
      if (input.size()>cursor)
        input.erase(cursor);
      break;
    }
    case TinyTerm::KEY_RETURN:
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
      break;
    }
		case TinyTerm::KEY_BACK:
		{
			if (input.size() and cursor>0)
				input.erase(--cursor, 1);
			break;
		}
		case TinyTerm::KEY_SUPPR:
		{
			if (cursor < input.size())
				input.erase(cursor, 1);
			break;
		}
		case TinyTerm::KEY_UP:
		{
      string start = input.substr(0, cursor);
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
      break;
		}
		case TinyTerm::KEY_DOWN:
	  {
	  	if (histo_n)
			{
				string start = input.substr(0, cursor);
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
      break;
		}
    case TinyTerm::KEY_RIGHT: if (cursor < input.size()) cursor++; break;
    case TinyTerm::KEY_LEFT: if (cursor > 0) cursor--; break;
    case TinyTerm::KEY_HOME: cursor=0; break;
    case TinyTerm::KEY_END: cursor=input.size(); break;
		default:
		{
    	if (key>=32 and key<=255)
     		input.insert(cursor++, 1, (char)key);
     	break;
		}
	}
	prompt();
}
