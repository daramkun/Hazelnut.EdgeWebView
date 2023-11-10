#ifndef EDGEWEBVIEW_EDGEWEBVIEW_H
#define EDGEWEBVIEW_EDGEWEBVIEW_H

#include <cstdint>

#if EXPORT_TO_DLL
#   define DLLEXPORT __declspec(dllexport)
#else
#   define DLLEXPORT __declspec(dllimport)
#endif

#define STDCALL __stdcall

extern "C" {
using window_t = void *;
using webview_t = void *;

using bool32_t = int32_t;
using unistring_t = const char16_t *;
using unichar_t = char16_t;

using windowcreated_callback = void (STDCALL *)(webview_t sender);
using windowclosing_callback = void (STDCALL *)(webview_t sender, bool32_t &cancel);
using windowclosed_callback = void (STDCALL *)(webview_t sender);
using navigating_callback = void (STDCALL *)(webview_t sender, unistring_t uri, bool32_t &cancel);
using navigated_callback = void (STDCALL *)(webview_t sender, bool succeeded, unistring_t uri, uint16_t status_code);

struct WebView2Options {
    window_t ParentWindowHandle;

    uint32_t Width;
    uint32_t Height;
    bool32_t Sizable;

    bool32_t CustomTitle;
    unichar_t Title[128];

    unichar_t UserAgent[256];
    unichar_t UserDataDirectoryPath[1024];

    windowcreated_callback WindowCreatedCallback;
    windowclosing_callback WindowClosingCallback;
    windowclosed_callback WindowClosedCallback;
    navigating_callback NavigatingCallback;
    navigated_callback NavigatedCallback;
};

enum WebView2ShowErrorCode {
    WebView2ShowErrorCode_NoError,

    WebView2ShowErrorCode_InvalidArguments,

    WebView2ShowErrorCode_NoRegisteredWindowClass,
    WebView2ShowErrorCode_NoCoInitializeCalled,
    WebView2ShowErrorCode_WindowSizeAdjustFailed,
    WebView2ShowErrorCode_NoWindowCreated,
    WebView2ShowErrorCode_NoWebView2ObjectCreated,
    WebView2ShowErrorCode_SoManyWebView2RuntimeDetected,
    WebView2ShowErrorCode_WebView2RuntimeVersionMismatch,
    WebView2ShowErrorCode_WebView2RuntimeNoInstalled,
    WebView2ShowErrorCode_UserDataFolderAccessDenied,
    WebView2ShowErrorCode_UnableToStartEdgeRuntime,
};

DLLEXPORT webview_t STDCALL CreateWebView2Window(const WebView2Options &options);
DLLEXPORT WebView2ShowErrorCode STDCALL ShowWebView2Window(webview_t webView, unistring_t uri);
DLLEXPORT void STDCALL CloseWebView2Window(webview_t webView);
DLLEXPORT void STDCALL DestroyWebView2Window(webview_t webView);
DLLEXPORT void STDCALL NavigateWebView2Window(webview_t webView, unistring_t uri);
}

#endif
