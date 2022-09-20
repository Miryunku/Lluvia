#pragma once

namespace t
{
    struct MyException
    {
        MyException(const wchar_t* msg, const wchar_t* filename, const uint32_t line);

        const wchar_t* m_msg;
        const wchar_t* m_filename;
        const uint32_t m_line;
    };

    struct MyHrException
    {
        MyHrException(const int32_t hr, const wchar_t* filename, const uint32_t line);

        const int32_t m_hr;
        const wchar_t* m_filename;
        const uint32_t m_line;
    };

    inline void throw_if_failed(int32_t hr);
}

#define LLV_EXCEPTION(msg) t::MyException(msg, __FILEW__, __LINE__)
#define LLV_HR_EXCEPTION(hr) t::MyHrException(hr, __FILEW__, __LINE__)