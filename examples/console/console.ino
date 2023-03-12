#include <TinyConsole.h>    // https://github.com/hsaturn/TinyConsole
#include <TinyStreaming.h>  // https://github.com/hsaturn/TinyMqtt

void onCommand(const std::string &cmd)
{
  if (cmd=="cls")
  {
    Console.cls();
    Console.gotoxy(4,1);
  }
  if (cmd=="reset")
  {
    Console.reset();
    ESP.reset();
  }
  if (cmd=="size")
  {
#if TINY_CONSOLE_AUTOSIZE
    Console << "Size=" << Console.sx << 'x' << Console.sy << endl;
#else
  Console << "Not available" << endl;
#endif
  }

  Console.saveCursor().gotoxy(2,1).fg(TinyConsole::red).bg(TinyConsole::white);
  Console << "Last command: (" << cmd << ')';
  Console.eraseEol().restoreCursor().bg(TinyConsole::black);
  if (cmd=="green") Console.fg(TinyConsole::green);
  if (cmd=="red") Console.fg(TinyConsole::red);
  if (cmd=="cyan") Console.fg(TinyConsole::cyan);
  if (cmd=="yellow") Console.fg(TinyConsole::yellow);
  if (cmd=="magenta") Console.fg(TinyConsole::magenta);
  if (cmd=="blue") Console.fg(TinyConsole::blue);
  if (cmd=="white") Console.fg(TinyConsole::white);
  if (cmd=="colors")
  {
    Console
      << TinyConsole::red << " red "
      << TinyConsole::green << "green "
      << TinyConsole::yellow << "yellow "
      << TinyConsole::magenta << "magenta "
      << TinyConsole::cyan << "cyan "
      << TinyConsole::blue << "blue "
      << TinyConsole::white << "white ";
  }
  if (cmd=="help")
  {
    Console << endl << endl;
    Console << "  cls    : clear screen" << endl;
    Console << "  colors : get list of available colors" << endl;
    Console << "  color  : change prompt color (get list of colors with 'colors')" << endl;
    Console << "  help   : this help" << endl;
    Console << "  reset  : reset the ESP" << endl;
    Console << endl;
    Console << "  try to press FN key, this should display the key in blue at top of the screen" << endl;
  }
  Console << endl;
}

void onFnKey(int fn)
{
  Console.saveCursor().gotoxy(3,1).fg(TinyConsole::blue).bg(TinyConsole::white);
  Console << "Fn Key : " << fn;
  Console.eraseEol().restoreCursor();
}

void setup()
{
  Serial.begin(115200);
  Console.begin(Serial);
  Console.setPrompt("> ");
  Console.title("TinyConsole");
  Console.setCallback(onCommand);
  Console.setCallbackFnKey(onFnKey);
  Console << endl << endl << endl << endl << endl;
  Console << "Welcome to TinyConsole minidemo" << endl;
  Console << "Enter 'help' to see available commands" << endl << endl;
  if (not Console.isTerm())
  {
    Console << endl;
    Console << "*** Warning ***: You are not using a real terminal. (no colors, no cursor etc..)";
    Console << endl << endl;
  }
  Console.prompt();
}

void loop()
{
  static int increment=0;
  static int counter=0;
  if (increment++>10000)
  {
    if (Console.isTerm())
    {
      increment=0;
      Console.saveCursor().gotoxy(4,1);
      Console << TinyConsole::cyan << "10k counter: " << counter++ << ' ';
      Console.restoreCursor();
    }
  }
  Console.loop();
}
