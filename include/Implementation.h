#ifndef EDGEWEBVIEW_IMPLEMENTATION_H
#define EDGEWEBVIEW_IMPLEMENTATION_H

#include "EdgeWebView.h"

#include <Windows.h>

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include <wrl.h>

class WebView2Window : public IUnknown
{
public:
    WebView2Window(const WebView2Options &options);
    virtual ~WebView2Window();

public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

public:
    WebView2ShowErrorCode Show(unistring_t uri);
    void Close();
    void Navigate(unistring_t uri);

public:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    ULONG _refCount;

    HWND _hWnd;

    Microsoft::WRL::ComPtr<ICoreWebView2> _webView;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> _controller;

    const WebView2Options _options;
};

#endif
