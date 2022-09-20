#include "pch.h"
#include "App.h"

int32_t wWinMain(HINSTANCE instance, HINSTANCE instance2, wchar_t* cmdline, int32_t cmdshow)
{
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
    {
        return -1;
    }
    int32_t exit_code = 0;
    bool init_success = true;
    {
        t::App app(instance);
        try
        {
            app.initialize();
        }
        catch (const std::exception& e)
        {
            init_success = false;
            exit_code = -2;
        }

        if (init_success)
        {
            ShowWindow(app.get_window(), cmdshow);

            MSG msg;
            while (GetMessage(&msg, nullptr, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            exit_code = msg.wParam;
        }
    }
    CoUninitialize();
    return exit_code;
}