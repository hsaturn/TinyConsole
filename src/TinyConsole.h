#pragma once
#include <string>
#include <deque>
#include <Stream.h>
#include <TinyTerm.h>

class TinyConsole
{
	public:
    using string = std::string;
    using CallBack = std::function<void(const string& command)>;

    TinyConsole(TinyTerm* term);

		// Callback when carriage return is hit with current line
    void setCallback(CallBack cb) { callback = cb; }

    void prompt() const;
		
    // nullptr prompt disable display
    void setPrompt(const char* prompt)
    { if (prompt) ps1 = prompt; else ps1.clear(); }

    void onKey(TinyTerm::KeyCode key);

    template<class T>
    friend TinyTerm& operator << (TinyConsole& con, T value)
		{
			*con.term << value;
			return *con.term;
		};

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

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL) && !defined(NO_GLOBAL_CONSOLE)
extern TinyConsole Console;
#endif
