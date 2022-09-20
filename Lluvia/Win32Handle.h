#pragma once

class Win32Handle {
public:
    Win32Handle(HANDLE handle);
    ~Win32Handle() noexcept;

    Win32Handle(const Win32Handle& other) = delete;
    Win32Handle& operator= (const Win32Handle& other) = delete;

    Win32Handle(Win32Handle&& other);
    Win32Handle& operator= (Win32Handle&& other);

    operator HANDLE ();
private:
    HANDLE m_handle = nullptr;
};