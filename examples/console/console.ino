#include <TinyConsole.h>    // https://github.com/hsaturn/TinyConsole
#include <TinyStreaming.h>  // https://github.com/hsaturn/TinyMqtt
#include <math.h>

float max(float a, float b)
{
  return a>b ? a : b;
}

std::string getWord(std::string& str, char sep = ' ')
{
	std::string word;
	auto spc = str.find(sep);
	if (spc == std::string::npos)
	{
		word = str;
		str = "";
	}
	else
	{
		word = str.substr(0, spc);
		str = str.substr(spc+1);
	}
	return word;
}

void spiral(int max_ray, int x, int y, int dr)
{
  Console.cls();
  float angle=0;
  float d_angle = 0.05;
  float ray=1;
  while(ray<max_ray)
  {
    Console.gotoxy(max(x+cos(angle)*ray,0.0), max(y+sin(angle)*ray*1.5, 0.0));
    Console << '*';
    angle+=d_angle;
    ray +=dr/200.0;
  }
}

void onCommand(const std::string &cmd_in)
{
  static std::string last;
  std::string args = cmd_in;
  if (args=="!")
  {
    args = last;
  }
  else if (cmd_in.length())
  {
    last = cmd_in;
  }
  std::string cmd=getWord(args);

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
    Console << "  spiral [ray] [x] [y] [dr]: draw a spiral (replace ray/x/y/dr by - to use previous value)" << endl;
    Console << "  !      : repeat the last command" << endl;
    Console << endl;
    Console << "  try to press FN key, this should display the key in blue at top of the screen" << endl;
  }
  static int s_r = 40;
  static int s_x = 20;
  static int s_y = 40;
  static int s_dr = 10;
  if (cmd=="spiral")
  {
    int ray=atoi(getWord(args).c_str());
    int x=atoi(getWord(args).c_str());
    int y=atoi(getWord(args).c_str());
    int dr=atoi(getWord(args).c_str());
    if (ray==0) ray=s_r;
    if (x==0) x=s_x;
    if (y==0) y=s_y;
    if (dr==0) dr=s_dr;
    s_r = ray;
    s_x = x;
    s_y = y;
    s_dr = dr;
    spiral(ray,x,y,dr);
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


