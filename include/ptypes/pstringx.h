
#ifndef __PSTRINGX_H__
#define __PSTRINGX_H__

#include <ptypes.h>

USING_PTYPES

string trim(string);
string implode(const string& separator, const variant& array);
variant explode(char separator, const string& s);
variant explode(const string& separator, const string& s);
string uppercase(const string& s);

#endif // __PSTRINGX_H__
