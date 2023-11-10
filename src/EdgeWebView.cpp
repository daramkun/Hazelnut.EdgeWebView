#include "EdgeWebView.h"
#include "Implementation.h"

webview_t CreateWebView2Window(const WebView2Options& options) {
    return new WebView2Window(options);
}

WebView2ShowErrorCode ShowWebView2Window(webview_t webView, unistring_t uri) {
    if (webView == nullptr) {
        return WebView2ShowErrorCode_InvalidArguments;
    }

    auto webViewWindow = reinterpret_cast<WebView2Window*>(webView);

    return webViewWindow->Show(uri);
}

void CloseWebView2Window(webview_t webView) {
    if (webView == nullptr) {
        return;
    }

    auto webViewWindow = reinterpret_cast<WebView2Window*>(webView);

    webViewWindow->Close();
}

void DestroyWebView2Window(webview_t webView) {
    if (webView == nullptr) {
        return;
    }

    auto webViewWindow = reinterpret_cast<WebView2Window*>(webView);
    delete webViewWindow;
}

void NavigateWebView2Window(webview_t webView, unistring_t uri) {
    if (webView == nullptr) {
        return;
    }

    auto webViewWindow = reinterpret_cast<WebView2Window*>(webView);

    webViewWindow->Navigate(uri);
}