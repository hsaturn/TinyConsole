#pragma once
#include <string>
#include <deque>
#include <Stream.h>
#include <TinyTerm.h>

class TinyConsole
{
	public:
    using string = std::string;
    using CallBack = void(*)(const string& command);

    TinyConsole(TinyTerm* term);

		// Callback when carriage return is hit with current line
    void setCallback(CallBack cb) { callback = cb; }

    void prompt() const;
		
    void setPrompt(const char* prompt)
    { ps1 = prompt; }

    void onKey(TinyTerm::KeyCode key);

    template<class T>
    friend TinyTerm& operator << (TinyConsole& con, T value)
		{
			*con.term << value;
			return *con.term;
		};

		// For compatibility, please do not use for new projects

    void begin(long baud) { term->begin(baud); }
    void begin(Stream& stream) { term->begin(stream); }

	private:
    string::size_type cursor=0; // column
    string input;
    string ps1;
    uint8_t histo_n = 0;
    uint8_t histo_max = 20;
    std::deque<string> history;
    CallBack callback;
    TinyTerm* term;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
extern TinyConsole Console;
#endif
