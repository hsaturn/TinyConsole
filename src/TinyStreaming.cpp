#include "TinyStreaming.h"

const char* hex(uint16_t i)
{
  static char h[]="0x0000";
  static const char* d="0123456789ABCDEF";
  for(int j=2; j<6; j++)
  {
    h[j]=d[(i & 0xF000) >> 12];
    i <<=4;
  }
  return h;
}