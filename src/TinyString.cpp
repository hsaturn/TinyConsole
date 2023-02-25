#include "TinyString.h"
#include <cstdlib>

const char* TinyString::emptyString = "";

TinyString::TinyString(const char* buffer, size_type s)
{
  dup(buffer, s);
}

TinyString::TinyString(int i, int base)
{
  reserve(sizeof(int)*8+1);
  itoa(i, str, base);
  size_ = strlen(str);
  free_ -= size_;
}

TinyString::TinyString(char c)
{
  reserve(2);
  str[0]=c;
  size_ = 1;
  free_ -= size_;
}

TinyString::TinyString(const TinyString& m)
{
  copy(m);
}

TinyString& TinyString::operator=(TinyString && other)
{
  if (this == &other) return *this;
  clear();
  std::swap(other.str, str);
  std::swap(other.size_, size_);
  std::swap(other.free_, free_);
  return *this;
}

TinyString& TinyString::operator=(const TinyString& m)
{
  copy(m);
  return *this;
}

TinyString& TinyString::operator+=(const char c)
{
  push_back(c);
  return *this;
}

TinyString operator +(const TinyString& s, const char c)
{
  TinyString result(s);
  result += c;
  return result;
}

TinyString operator +(const char c, const TinyString& s)
{
  TinyString result(c);
  result += s;
  return result;
}

TinyString operator+(const TinyString& left, const TinyString& right)
{
  TinyString result(left);
  result += right;
  return result;
}

TinyString& TinyString::operator +=(int i)
{
  reserve(size_ + sizeof(int)*3+1);
  itoa(i, str + size_, 10);
  int8_t sz = strlen(str+size_);
  size_ += sz;
  free_ -= sz;
  return *this;
}

void TinyString::concat(const char* buf, size_type len)
{
  reserve(size_ + len + 1);
  strcpy(str + size_, buf);
  size_ += len;
  free_ -= len;
}

void TinyString::push_back(const char c)
{
  reserve(size_+1, extent);
  str[size_++] = c;
  str[size_] = 0;
  free_--;
}

void TinyString::erase(size_type pos, size_type size)
{
  if (size == npos) size = size_;
  if (pos > size_) return;
  if (pos + size > size_) size = size_ - pos;
  memmove(str+pos, str+pos+size, size_ - pos + 1);
  if (size_ == size)
  {
    clear();
  }
  else
  {
    size_ -= size;
    free_ += size;
  }
}

void TinyString::clear()
{
  if (size_)
    free(str);
  str = const_cast<char*>(emptyString);  // Dangerous str must be left untouched when size_ == 0
  size_ = 0;
}

TinyString& TinyString::operator = (const char c)
{
  dup(&c, 1);
  return *this;
}

TinyString TinyString::substr(size_type pos, size_type size)
{
  if (size == npos) size = size_;
  if (pos > size_) return TinyString();
  if (pos + size > size_) size = size_ - pos;
  return TinyString(str+pos, size);
}

bool TinyString::starts_with(const char* buf, size_type size) const
{
  const_iterator it(str);
  while(size and it != end() and (*it == *buf))
  {
    it++;
    buf++;
    size--;
  }
  return size == 0;
}

// Bug: unit test is ok under Kubuntu but not under github...
#define BUG_SEARCH 0
#if BUG_SEARCH
#include <iostream>
#include <iomanip>
void dump(const char* s, size_type len)
{
  const char* s1 = s;
  std::cout << "  dump ";
  len++;
  while (len--)
  {
    std::cout << std::setw(2) << std::hex << (int)(*s++) << ' ';
  }
  std::cout << " : " << s1 << std::endl;
}
#endif

int8_t TinyString::compare(const char* s, size_type len) const
{
#if BUG_SEARCH
  std::cout << "compare " << s << ':' << len << " with " << str << ':' << size_ << std::endl;
  dump(str, size_);
  dump(s, len);
#endif
  if (len > size_)
    return memcmp(str, s, size_ + 1);
  else
    return memcmp(str, s, len + 1);
}

void TinyString::reserve(size_type sz, uint8_t extent)
{
  if (sz == 0)
  {
    clear();
    return;
  }
  if (size_ == 0)
  {
    free_ = sz + extent;
    str = static_cast<char*>(malloc(sz + extent));
    return;
  }
  if ((sz > size_ + free_) or (extent > size_ + free_ - sz))
  {
    free_ = sz + extent - size_;
    str = static_cast<char*>(realloc(str, sz + extent));
  }
}

void TinyString::collect()
{
  if (size_ > 0 and free_ > 1)
  {
    str = static_cast<char*>(realloc(str, size_ + 1));
    free_ = 1;
  }
}

TinyString::size_type TinyString::find(const char c, size_type from) const
{
  if (size_ == 0 or from > size_) return npos;

  const char* f = str + from;
  const char* end = str + size_;
  while(f != end) if (*f++ == c) return f-str-1;

  return npos;
}

TinyString::size_type TinyString::find(const char* p, size_type psize, size_type from) const
{
  if (size_ == 0 or from > size_) return npos;
  if (size_ - from < psize) return npos;
  size_type left = size_ - from;
  const char* f = str + from;
  for(;;)
  {
    if (memcmp(f, p, psize) == 0) return f-str;
    if (left-- == 0) return npos;
    f++;
  }
}

TinyString::size_type TinyString::find_first_not_of(char c, size_type from) const
{
  if (size_ == 0 or from > size_) return npos;
  const char* f = str+from;
  size_type left = size_ - from;
  for(;;)
  {
    if (left-- == 0) return npos;
    if (*f != c) return f-str;
    f++;
  }
}

TinyString::size_type TinyString::find_last_not_of(char c, size_type from) const
{
  if (from == npos) from = size_;
  if (size_ == 0 or from > size_) return npos;
  for(const char* f=str+from-1; f>= str; f--)
  {
    if (*f != c) return f-str;
  }
  return npos;
}

void TinyString::dup(const char* buffer, size_type sz, uint8_t extent)
{
  reserve(sz + 1, extent);
  memcpy(str, buffer, sz);
  str[sz] = 0;
  if (size_ > sz)
    free_ += size_ - sz;
  else
    free_ -= sz - size_;
  size_ = sz;
}

void TinyString::insert(size_type pos, size_type n, char c)
{
  if (pos > size_) pos = size_;
  reserve(size_ + n, 1);
  char* start = str+pos;
  if (size_ > pos)
  {
    memmove(start+n, start, size_ - pos + 1);
  }
  else
    str[size_ + n ] = 0;
  assert(free_ >= n);
  size_ += n;
  free_ -= n;
  while(n-->0) *start++=c;
}

void TinyString::insert(size_type pos, const char* s, size_type size)
{
  if (pos > size_) pos = size_;
  reserve(size_ + size, 1);
  char* dest = str+pos;
  if (size_ > pos)
    memmove(dest+size, dest, size_ - pos + 1);
  else
    str[size_ + size ] = 0;
  assert(free_ > size);
  size_ += size;
  free_ -= size;
  memcpy(dest, s, size);
}
