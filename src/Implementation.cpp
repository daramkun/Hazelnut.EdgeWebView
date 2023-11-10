#include "Implementation.h"
#include "wil/resource.h"

#include <unordered_map>

#include <wil/safecast.h>

#define WINDOW_CLASSNAME TEXT("EdgeWebViewWindow")

struct WindowClass {
public:
    WindowClass() {
        _hInstance = GetModuleHandle(nullptr);

        WNDCLASS wndClass = {
                CS_HREDRAW | CS_VREDRAW,
                WebView2Window::WndProc,
                0,
                0,
                _hInstance,
                LoadIcon(_hInstance, IDI_APPLICATION),
                LoadCursor(nullptr, IDC_ARROW),
                nullptr,
                nullptr,
                WINDOW_CLASSNAME,
        };

        _registerResult = RegisterClass(&wndClass);
    }

    ~WindowClass() {
        UnregisterClass(WINDOW_CLASSNAME, _hInstance);
    }

public:
    BOOL IsRegisterSucceed() const { return _registerResult != INVALID_ATOM; }

    HINSTANCE GetInstanceHandle() const { return _hInstance; }

private:
    HINSTANCE _hInstance;
    ATOM _registerResult;
} g_registerWindow;

struct COMInitializer {
public:
    COMInitializer() {
        _result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    }
    ~COMInitializer() {
        CoUninitialize();
    }

public:
    BOOL IsInitialized() { return SUCCEEDED(_result); }

private:
    HRESULT _result;
} g_comInitializer;

WebView2Window::WebView2Window(const WebView2Options &options)
        : _refCount(1), _options(options), _hWnd(nullptr) {

}

WebView2Window::~WebView2Window() {
    if (_hWnd != nullptr) {
        Close();
    }
}

HRESULT STDMETHODCALLTYPE WebView2Window::QueryInterface(const IID &riid, void **ppvObject) {
    return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE WebView2Window::AddRef() {
    return InterlockedIncrement(&_refCount);
}

ULONG STDMETHODCALLTYPE WebView2Window::Release() {
    const auto result = InterlockedDecrement(&_refCount);
    if (result == 0) {
        delete this;
    }

    return result;
}

WebView2ShowErrorCode WebView2Window::Show(unistring_t uri) {
    if (_hWnd != nullptr) {
        Navigate(uri);
        return WebView2ShowErrorCode_NoError;
    }

    if (!g_registerWindow.IsRegisterSucceed()) {
        return WebView2ShowErrorCode_NoRegisteredWindowClass;
    }

    if (!g_comInitializer.IsInitialized()) {
        return WebView2ShowErrorCode_NoCoInitializeCalled;
    }

    const auto windowStyle = _options.Sizable
                             ? (WS_OVERLAPPEDWINDOW)
                             : (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
    const auto dpiScale = GetDpiForSystem() / 96.0;

    RECT clientRect = {0, 0, (LONG) (_options.Width * dpiScale), (LONG) (_options.Height * dpiScale)};
    if (!AdjustWindowRect(&clientRect, windowStyle, FALSE)) {
        return WebView2ShowErrorCode_WindowSizeAdjustFailed;
    }

    const auto width = clientRect.right - clientRect.left,
            height = clientRect.bottom - clientRect.top;
    const auto x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
            y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

    const auto windowName = !_options.CustomTitle ? u"" : _options.Title;

    _hWnd = CreateWindow(WINDOW_CLASSNAME, (LPCWSTR) windowName,
                         windowStyle, x, y, width, height,
                         (HWND) _options.ParentWindowHandle, nullptr,
                         g_registerWindow.GetInstanceHandle(), this);
    if (_hWnd == nullptr) {
        return WebView2ShowErrorCode_NoWindowCreated;
    }

    ShowWindow(_hWnd, SW_SHOW);
    UpdateWindow(_hWnd);

    const auto userDataFolder = (LPCWSTR) _options.UserDataDirectoryPath;
    auto environmentOptions = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    Microsoft::WRL::ComPtr<ICoreWebView2Environment2> environmentOptions2;
    environmentOptions.As<ICoreWebView2Environment2>(&environmentOptions2);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment3> environmentOptions3;
    environmentOptions.As<ICoreWebView2Environment3>(&environmentOptions3);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment4> environmentOptions4;
    environmentOptions.As<ICoreWebView2Environment4>(&environmentOptions4);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment5> environmentOptions5;
    environmentOptions.As<ICoreWebView2Environment5>(&environmentOptions5);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment6> environmentOptions6;
    environmentOptions.As<ICoreWebView2Environment6>(&environmentOptions6);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment7> environmentOptions7;
    environmentOptions.As<ICoreWebView2Environment7>(&environmentOptions7);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment8> environmentOptions8;
    environmentOptions.As<ICoreWebView2Environment8>(&environmentOptions8);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment9> environmentOptions9;
    environmentOptions.As<ICoreWebView2Environment9>(&environmentOptions9);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment10> environmentOptions10;
    environmentOptions.As<ICoreWebView2Environment10>(&environmentOptions10);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment11> environmentOptions11;
    environmentOptions.As<ICoreWebView2Environment11>(&environmentOptions11);
    Microsoft::WRL::ComPtr<ICoreWebView2Environment12> environmentOptions12;
    environmentOptions.As<ICoreWebView2Environment12>(&environmentOptions12);

    const auto environmentCompleted =
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this, uri](HRESULT errorCode, ICoreWebView2Environment *createdEnvironment) -> HRESULT {
                auto controllerCompleted =
                    Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this, uri](HRESULT errorCode, ICoreWebView2Controller *createdController) -> HRESULT {
                            if (FAILED(errorCode)) {
                                return errorCode;
                            }

                            _controller = createdController;

                            if (FAILED(errorCode = _controller->get_CoreWebView2(&_webView))) {
                                return errorCode;
                            }

                            Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;
                            if (SUCCEEDED(_webView->get_Settings(&settings))) {
                                Microsoft::WRL::ComPtr<ICoreWebView2Settings2> settings2;
                                if (SUCCEEDED(settings.As<ICoreWebView2Settings2>(&settings2))) {
                                    if (_options.UserAgent[0] != '\0') {
                                        settings2->put_UserAgent((LPCWSTR) _options.UserAgent);
                                    }
                                }

                                Microsoft::WRL::ComPtr<ICoreWebView2Settings3> settings3;
                                settings.As<ICoreWebView2Settings3>(&settings3);

                                Microsoft::WRL::ComPtr<ICoreWebView2Settings4> settings4;
                                settings.As<ICoreWebView2Settings4>(&settings4);

                                Microsoft::WRL::ComPtr<ICoreWebView2Settings5> settings5;
                                settings.As<ICoreWebView2Settings5>(&settings5);

                                Microsoft::WRL::ComPtr<ICoreWebView2Settings6> settings6;
                                settings.As<ICoreWebView2Settings6>(&settings6);

                                Microsoft::WRL::ComPtr<ICoreWebView2Settings7> settings7;
                                settings.As<ICoreWebView2Settings7>(&settings7);

                                Microsoft::WRL::ComPtr<ICoreWebView2Settings8> settings8;
                                settings.As<ICoreWebView2Settings8>(&settings8);
                            }

                            {
                                RECT clientRect;
                                GetClientRect(_hWnd, &clientRect);
                                _controller->put_Bounds(clientRect);
                            }

                            {
                                auto navigationStarting = Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
                                    [this](ICoreWebView2 *sender, ICoreWebView2NavigationStartingEventArgs *args) -> HRESULT {
                                        bool32_t cancel = false;
                                        if (_options.NavigatingCallback) {
                                            wil::unique_cotaskmem_string uri;
                                            if(SUCCEEDED(args->get_Uri(&uri))) {
                                                _options.NavigatingCallback(this, (unistring_t) uri.get(), cancel);
                                            }
                                        }
                                        args->put_Cancel(cancel);

                                        return S_OK;
                                    });
                                _webView->add_NavigationStarting(navigationStarting.Get(), nullptr);

                                auto navigationCompleted = Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
                                    [this](ICoreWebView2 *sender, ICoreWebView2NavigationCompletedEventArgs *args) -> HRESULT {
                                        int httpStatusCode = 200;

                                        Microsoft::WRL::ComPtr<ICoreWebView2NavigationCompletedEventArgs2> args2;
                                        if (SUCCEEDED(args->QueryInterface<ICoreWebView2NavigationCompletedEventArgs2>(&args2))) {
                                            args2->get_HttpStatusCode(&httpStatusCode);
                                        }

                                        BOOL succeed = false;
                                        if (FAILED(args->get_IsSuccess(&succeed)) || !succeed) {
                                            COREWEBVIEW2_WEB_ERROR_STATUS errorStatus;
                                            args->get_WebErrorStatus(&errorStatus);

                                            if (_options.NavigatedCallback) {
                                                _options.NavigatedCallback(this, false, nullptr, httpStatusCode);
                                            }
                                        } else {
                                            if (_options.NavigatedCallback) {
                                                wil::unique_cotaskmem_string uri;
                                                if(SUCCEEDED(_webView->get_Source(&uri))) {
                                                    _options.NavigatedCallback(this, true, (unistring_t) uri.get(), httpStatusCode);
                                                }
                                            }

                                            if (!_options.CustomTitle) {
                                                wil::unique_cotaskmem_string title;
                                                sender->get_DocumentTitle(&title);
                                                SetWindowText(_hWnd, title.get());
                                            }
                                        }

                                        return S_OK;
                                    });
                                _webView->add_NavigationCompleted(navigationCompleted.Get(), nullptr);
                            }

                            if (uri != nullptr) {
                                Navigate(uri);
                            }

                            if (_options.WindowCreatedCallback) {
                                _options.WindowCreatedCallback(this);
                            }

                            return S_OK;
                        });

                return createdEnvironment->CreateCoreWebView2Controller(_hWnd, controllerCompleted.Get());
            });

    const auto result = CreateCoreWebView2EnvironmentWithOptions(nullptr, userDataFolder,
                                                                 environmentOptions.Get(),
                                                                 environmentCompleted.Get());

    if (FAILED(result)) {
        switch (result) {
            case CO_E_NOTINITIALIZED: return WebView2ShowErrorCode_NoCoInitializeCalled;
            case RPC_E_CHANGED_MODE: return WebView2ShowErrorCode_NoCoInitializeCalled;
            case HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED): return WebView2ShowErrorCode_NoWebView2ObjectCreated;
            case HRESULT_FROM_WIN32(ERROR_INVALID_STATE): return WebView2ShowErrorCode_InvalidArguments;
            case HRESULT_FROM_WIN32(ERROR_DISK_FULL): return WebView2ShowErrorCode_SoManyWebView2RuntimeDetected;
            case HRESULT_FROM_WIN32(ERROR_PRODUCT_UNINSTALLED): return WebView2ShowErrorCode_WebView2RuntimeVersionMismatch;
            case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND): return WebView2ShowErrorCode_WebView2RuntimeNoInstalled;
            case E_ACCESSDENIED: return WebView2ShowErrorCode_UserDataFolderAccessDenied;
            case E_FAIL: return WebView2ShowErrorCode_UnableToStartEdgeRuntime;
        }
    }

    return WebView2ShowErrorCode_NoError;
}

void WebView2Window::Close() {
    if (_hWnd == nullptr) {
        return;
    }

    CloseWindow(_hWnd);
    DestroyWindow(_hWnd);

    _hWnd = nullptr;
}

void WebView2Window::Navigate(unistring_t uri) {
    OutputDebugString((LPCWSTR)uri);
    OutputDebugString(L"\n");
    _webView->Navigate((LPCWSTR) uri);
}

LRESULT WebView2Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::unordered_map<HWND, WebView2Window *> windows;

    const auto found = windows.find(hWnd);
    auto window = found != windows.end() ? found->second : nullptr;

    if (uMsg != WM_CREATE && window == nullptr) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_CREATE: {
            const auto cs = reinterpret_cast<const CREATESTRUCT *>(lParam);
            auto createdWindow = reinterpret_cast<WebView2Window *>(cs->lpCreateParams);
            windows.insert(std::pair<HWND, WebView2Window *>(hWnd, createdWindow));
            break;
        }

        case WM_SIZE: {
            if (window->_controller != nullptr) {
                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                window->_controller->put_Bounds(clientRect);
            }
            break;
        }

        case WM_DPICHANGED: {
            RECT* const newWindowSize = reinterpret_cast<RECT*>(lParam);
            SetWindowPos(
                    hWnd, nullptr, newWindowSize->left, newWindowSize->top,
                    newWindowSize->right - newWindowSize->left,
                    newWindowSize->bottom - newWindowSize->top, SWP_NOZORDER | SWP_NOACTIVATE);

            if (window->_controller != nullptr) {
                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                window->_controller->put_Bounds(clientRect);
            }
            break;
        }

        case WM_CLOSE: {
            if (window->_options.WindowClosingCallback) {
                bool32_t cancel = false;
                window->_options.WindowClosingCallback(window, cancel);

                if (!cancel) {
                    DestroyWindow(hWnd);
                }
            } else {
                DestroyWindow(hWnd);
            }
            break;
        }

        case WM_DESTROY: {
            if (window->_options.WindowClosedCallback) {
                window->_options.WindowClosedCallback(window);
            }

            window->_hWnd = nullptr;
            window->_webView = nullptr;
            window->_controller = nullptr;

            windows.erase(found);

            break;
        }

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}
