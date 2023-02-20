#pragma once
#include <Arduino.h>
#include <assert.h>
#include <cstdint>

#pragma pack(push, 1)

class TinyString
{
  public:
    using value_type = char;
    using size_type = int16_t;
    static constexpr size_type npos = std::numeric_limits<size_type>::max();

    TinyString() = default;
    TinyString(int, int base=10);
    TinyString(char);
    TinyString(const TinyString&);
    TinyString(const char*, size_type size);
    TinyString(const char* s) : TinyString(s, strlen(s)){};
    TinyString& operator= (const TinyString&);
    TinyString& operator= (TinyString&&);
    ~TinyString() { clear(); }

    int8_t compare(const char* buf, size_type len) const;
    int8_t compare(const char* buf) const { return compare(buf, strlen(buf)); }

    friend bool operator == (const TinyString& l, const TinyString& r) { return l.compare(r, r.size_) == 0; }
    friend bool operator == (const TinyString& l, const char* r) { return l.compare(r) == 0; }
    friend bool operator != (const TinyString& l, const char* r) { return not (l == r); }
    friend bool operator < (const TinyString& l, const TinyString& r) { return l.compare(r, r.size_) <0; }

    const char* c_str() const { return str; }
    size_type length() const { return size_; }
    size_type size() const { return length(); }
    void concat(const char* buf, size_type len);
    void insert(size_type pos, size_type n, char c);
    void insert(size_type pos, char c) { insert(pos, 1, c); }
    void insert(size_type pos, const char* p) { insert(pos, p, strlen(p)); }
    void insert(size_type pos, const char* p, size_type size);

    bool starts_with(const char* buf, size_type len) const;
    bool starts_with(const char* buf) const { return starts_with(buf, strlen(buf)); }

    size_type find(const char c, const size_type from=0) const;
    size_type find(const char* buf, const size_type from=0) const { return find(buf, strlen(buf), from); }
    size_type find(const char* buf, const size_type bufsize, const size_type from=0) const;
    size_type find(const TinyString& s, const size_type from=0) const { return find(s.str, s.size_, from); }
    size_type find_first_not_of(char c, size_type from=0) const;
    size_type find_last_not_of(char c, size_type from=npos) const;

    TinyString substr(size_type pos, size_type len = npos);

    // char& operator[](size_type index) { assert(index < size_); return str[index]; }
    TinyString& operator = (const char c);
    TinyString& operator +=(const char c);
    TinyString& operator +=(const char* buf) { concat(buf, strlen(buf)); return *this; }
    TinyString& operator +=(const TinyString& s) { concat(s.str, s.size_); return *this; }
    TinyString& operator +=(int32_t);

    friend TinyString operator+(const char, const TinyString&);
    friend TinyString operator+(const TinyString&, const char);
    friend TinyString operator+(const TinyString& left, const TinyString &right);

    operator const char*() const { return str; }
    operator char*() { return str; }

    void reserve(size_type size) { reserve(size, 0); }

    void erase(size_type pos, size_type size = npos);

    void dup(const char* buffer, size_type size, uint8_t extent = 4);

    void push_back(const char c);
    void clear();

    using const_iterator = const char*;
    using iterator = char*;
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    iterator begin() const { return str; }
    iterator end() const { return str + size_; }

    size_type capacity() const { return size_ + free_; }
    void collect(); // Save memory

  private:
    void reserve(size_type new_size, uint8_t extent);
    void copy(const TinyString& t) { dup(t.str, t.size_); };

    char* str = const_cast<char *>(emptyString);
    size_type size_ = 0;   // if size_ == 0 no allocation, but str = emptyString
    uint8_t free_ = 0;    // malloc(str) = size_ + free_

    static const char* emptyString;
    const uint8_t extent = 8;
};

#pragma pack(pop)
