#include "pch.h"
#include "MyExceptions.h"

t::MyException::MyException(const wchar_t* msg, const wchar_t* filename, const uint32_t line)
	: m_msg(msg), m_filename(filename), m_line(line)
{
}

/*
       lllllllllllllllllllllll
      lllllllllllllllllllllllll
     llllllllll       llllllllll
    llllllllll         llllllllll
   lllllllllllllllllllllllllllllll
  lllllllllllllllllllllllllllllllll
 llllllllll               llllllllll
llllllllll                 llllllllll
*/

t::MyHrException::MyHrException(const int32_t hr, const wchar_t* filename, const uint32_t line)
	: m_hr(hr), m_filename(filename), m_line(line)
{
}

/*
       lllllllllllllllllllllll
      lllllllllllllllllllllllll
     llllllllll       llllllllll
    llllllllll         llllllllll
   lllllllllllllllllllllllllllllll
  lllllllllllllllllllllllllllllllll
 llllllllll               llllllllll
llllllllll                 llllllllll
*/

inline void t::throw_if_failed(int32_t hr)
{
    if (FAILED(hr))
    {
        throw LLV_HR_EXCEPTION(hr);
    }
}