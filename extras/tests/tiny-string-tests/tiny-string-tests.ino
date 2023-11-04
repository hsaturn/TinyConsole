// vim: ts=2 sw=2 expandtab
#include <Arduino.h>
#include <AUnit.h>
#include <TinyString.h>
#include <string>

/**
  * TinyString unit tests.
  *
  **/

using namespace std;

test(_sizeof)
{
  cout << "-----------" << endl;
  cout << "Test sizeof std::string = " << sizeof(std::string) << endl;
  cout << "Test sizeof String      = " << sizeof(String) << endl;
  cout << "Test sizeof TinyString  = " << sizeof(TinyString) << endl;
  cout << "-----------" << endl;
}

test(ctor)
{
  TinyString s;
  assertEqual(s.size(), 0);

  TinyString s2("abcd");
  assertEqual("abcd", s2.c_str());

  TinyString s3(s2);
  assertEqual("abcd", s3);

  TinyString s4("abcdefgh", 4);
  assertEqual("abcd", s4);

  TinyString s5(s4);
  assertEqual("abcd", s5);

  TinyString s6(100);
  assertEqual(s6, "100");

  TinyString s7(-100);
  assertEqual(s7, "-100");

  TinyString bin(11, 2);
  assertEqual(bin, "1011");
}

test(clear)
{
  TinyString s("abcd");
  s.clear();
  assertEqual(s.size(), 0);
  assertEqual(s, "");
}

test(operator_friend_add)
{
  TinyString s("one");
  assertEqual(s+'.', TinyString("one."));
  assertEqual('.'+TinyString("one"), TinyString(".one"));

  TinyString t("two");
  assertEqual(s+t, TinyString("onetwo"));
}

test(operator_eq)
{
 TinyString s("abcd");
 TinyString t("abcd");
 TinyString u("uvwx");

 assertTrue(s==t);
 assertFalse(s==u);

 s='z';
 const char* p = s.c_str();
 assertEqual(s.size(), 1);
 assertEqual(s[0], 'z');
 assertEqual((int)p[1], 0);
 assertEqual(u, "uvwx");

 TinyString u2 = std::move(u);
 assertEqual(u2, "uvwx");
 assertEqual(not (u2 == "no..."), true);
}

test(operator_add)
{
  TinyString s("a");
  s += 'b';

  assertEqual(s, "ab");

  s += "cd";

  assertEqual(s, "abcd");

  TinyString ef("ef");

  s += ef;

  assertEqual(s, "abcdef");
}

test(operator_bracket)
{
  TinyString s("abcd");

  assertEqual(s[0], 'a');
  assertEqual(s[1], 'b');
  assertEqual(s[2], 'c');
  assertEqual(s[3], 'd');

  // Assignment
  s[1]='Z';
  assertEqual(s, "aZcd");

  // Or operator
  s[1] |= ' ';
  assertEqual(s, "azcd");
  uint8_t c=32;

  s[0] = 'A';
  s[0] |= c;
  assertEqual(s, "azcd");
}

test(substr)
{
  TinyString s("hello world");

  assertEqual(s.substr(0,5), "hello");
  assertEqual(s.substr(6,5), "world");
  assertEqual(s.substr(6), "world");
  assertEqual(s.substr(0,0), "");
}

test(starts_with)
{
  TinyString s("hello world");
  assertTrue(s.starts_with("hello"));
  assertFalse(s.starts_with("oops"));
}

test(reserve)
{
  TinyString s("ab");

  s.reserve(50);
  
  assertEqual(s.size(), 2);
  assertEqual(s.capacity(), 50);

  // shrink does nothing
  s.reserve(10);
  assertEqual(s.capacity(), 50);
}

test(collect)
{
  TinyString s("abcd");
  s.reserve(50);
  s.collect();
  assertEqual(s.capacity(), 5);
}

test(iterator)
{
  std::string r;
  TinyString s("abcdef");

  for(auto c: s)
  {
    r += c;
  }

  assertEqual(r.c_str(), s);
}

test(erase)
{
  TinyString s("abcdefgh");
  s.erase(0,1);
  assertEqual(s, "bcdefgh");
  s.erase(1,2);
  assertEqual(s, "befgh");
  s.erase(1);
  assertEqual(s, "b");
}

test(size)
{
  TinyString s("abcd");
  assertEqual(s.size(), 4);
  s.clear();
  assertEqual(s.size(), 0);
  assertEqual(s.size(), s.length());
}

test(concat)
{
  TinyString s("1");
  s+=1;
  assertEqual(s, "11");
}

test(compare)
{
  TinyString a("a");
  TinyString b("b");
  TinyString aa("aa");

  assertEqual(b.compare(b), 0);
  assertEqual(a.compare(b), -1);
  assertEqual(a.compare(aa), -97);
  assertEqual(aa.compare(a), 97);
  assertEqual(a=="a", true);

  assertTrue(b > a);
  assertFalse(b > b);
  assertFalse(a > b);
}

test(find)
{
  TinyString e;
  assertEqual(e.find('a'), TinyString::npos);

  TinyString s("abcdefgh");

  assertEqual(s.find('z'), TinyString::npos);
  assertEqual(s.find('a'), 0);
  assertEqual(s.find('\0'), TinyString::npos);

  assertEqual(s.find('a', 0), 0);
  assertEqual(s.find('a', 1), TinyString::npos);

  assertEqual(s.find('a', 5555), TinyString::npos);

  s[0] = '\0';
  assertEqual(s.find('\0'), 0);
  assertEqual(s.find("bcd"), 1);

  TinyString needle("efgh");

  auto pos = s.find(needle);
  assertEqual(pos, 4);

  needle[1] = '\0';
  s[pos+1] = '\0';
  assertEqual(s.find(needle), pos);
  s[pos+2]='-';
  assertEqual(s.find(needle), TinyString::npos);
}

test(find_first_not_of)
{
  TinyString s("   abc");
  assertEqual(s.find_first_not_of(' '), 3);

  s="";
  assertEqual(s.find_first_not_of(' '), TinyString::npos);

  s="   ";
  assertEqual(s.find_first_not_of(' '), TinyString::npos);
}

test(find_last_not_of)
{
  TinyString s("   abc   ");
  assertEqual(s.find_last_not_of(' '), 5);
  assertEqual(s.find_last_not_of(' ', 2), TinyString::npos);
}

test(map)
{
  std::map<TinyString, int> m;
  TinyString s1("one");
  TinyString s2("two");
  TinyString s3("three");

  m[s1]++;
  m[s1]++;
  m[s2]++;

  assertEqual(m[s1], 2);
  assertEqual(m[s2], 1);
  assertEqual(m[s3], 0);
}

test(insert)
{
  TinyString s="0123456789";
  s.insert(5,1,'a');
  assertEqual(s, "01234a56789");

  s.insert(5, 'a');
  assertEqual(s, "01234aa56789");

  s.insert(0, 'a');
  assertEqual(s, "a01234aa56789");

  s.insert(s.length(), 'a');
  assertEqual(s, "a01234aa56789a");

  s="0123456789";
  assertEqual(s, "0123456789");
  s.insert(5, "-----");
  assertEqual(s, "01234-----56789");
  s.insert(0, "===");
  assertEqual(s, "===01234-----56789");
  s.insert(s.length(), "-end");
  assertEqual(s, "===01234-----56789-end");
}

//----------------------------------------------------------------------------
// setup() and loop()
void setup() {
  delay(1000);
  Serial.begin(115200);
  while(!Serial);

  Serial.println("=============[ TinyMqtt StringIndexer TESTS ]========================");
}

void loop() {
  aunit::TestRunner::run();

  // if (Serial.available()) ESP.reset();
}
