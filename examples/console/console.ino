#include <TinyConsole.h>    // https://github.com/hsaturn/TinyConsole
#include <MqttStreaming.h>  // https://github.com/hsaturn/TinyMqtt

void onCommand(const std::string &cmd)
{
  if (cmd=="cls") Console.cls();
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
  Console << "Last command: (" << cmd << ')' << endl;
  Console.eraseEol().restoreCursor();
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
}

void loop()
{
  static int increment=0;
  static int counter=0;
  if (increment++>10000)
  {
    increment=0;
    // Console.printAt(1,1, counter++);
  }
  Console.loop();
}

