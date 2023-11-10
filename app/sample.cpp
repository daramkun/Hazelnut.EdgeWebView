#include <Windows.h>
#include <EdgeWebView.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    WebView2Options options = {};
    options.Sizable = true;
    options.Width = 1280;
    options.Height = 720;
    options.WindowClosedCallback = [](webview_t webView) {
        PostQuitMessage(0);
    };

    auto webView = CreateWebView2Window(options);
    ShowWebView2Window(webView, u"https://daram.in");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseWebView2Window(webView);
    DestroyWebView2Window(webView);

    return 0;
}