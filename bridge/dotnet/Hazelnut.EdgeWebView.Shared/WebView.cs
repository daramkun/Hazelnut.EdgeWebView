using System.Collections.Concurrent;
using System.ComponentModel;
using System.Net;
using System.Runtime.InteropServices;

namespace Hazelnut.EdgeWebView;

public partial class WebView : IDisposable
{
    private static readonly ConcurrentDictionary<IntPtr, WebView> _webViewDict = new();
    
    private IntPtr _webView;

    public event EventHandler? WindowCreated;
    public event EventHandler<CancelEventArgs>? WindowClosing;
    public event EventHandler? WindowClosed;
    public event EventHandler<NavigatingEventArgs>? Navigating;
    public event EventHandler<NavigatedEventArgs>? Navigated;

    public WebView(in WebViewOptions options)
    {
        InitializeModule();
        Initialize(options);
    }

    ~WebView()
    {
        Dispose(false);
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
        if (_webView == IntPtr.Zero)
            throw new ObjectDisposedException(GetType().Name);
        
        DestroyWebView2Window!(_webView);

        _webViewDict.TryRemove(_webView, out _);
        _webView = IntPtr.Zero;
    }

    private unsafe void Initialize(in WebViewOptions options)
    {
        var nativeOptions = new WebView2Options();

        nativeOptions.ParentWindowHandle = options.ParentWindowHandle;
        
        nativeOptions.CustomTitle = options.UseCustomTitle ? 1 : 0;
        if (options.CustomTitle != null)
            CopyString(nativeOptions.Title, options.CustomTitle);

        nativeOptions.Width = options.Width != 0 ? (uint)options.Width : 1280;
        nativeOptions.Height = options.Height != 0 ? (uint)options.Height : 720;
        nativeOptions.Sizable = options.IsSizable ? 1 : 0;

        if (options.UserAgent != null)
            CopyString(nativeOptions.UserAgent, options.UserAgent);
        if (options.UserDataPath != null)
            CopyString(nativeOptions.UserDataDirectoryPath, options.UserDataPath);

        nativeOptions.WindowCreatedCallback += WindowCreatedCallback;
        nativeOptions.WindowClosingCallback += WindowClosingCallback;
        nativeOptions.WindowClosedCallback += WindowClosedCallback;
        nativeOptions.NavigatingCallback += NavigatingCallback;
        nativeOptions.NavigatedCallback += NavigatedCallback;

        _webView = CreateWebView2Window!(in nativeOptions);
        
        _webViewDict.TryAdd(_webView, this);
    }

    private unsafe void CopyString(char* destination, string source)
    {
        for (var i = 0; i < source.Length; ++i)
            destination[i] = source[i];
        destination[source.Length] = '\0';
    }

    public void Show(string? uri)
    {
        if (_webView == IntPtr.Zero)
            throw new ObjectDisposedException(GetType().Name);

        switch (ShowWebView2Window!(_webView, uri))
        {
            case WebView2ShowError.NoError:
                break;

            case WebView2ShowError.InvalidArguments: throw new ArgumentNullException(nameof(_webView));

            case WebView2ShowError.NoRegisteredWindowClass: throw new InvalidOperationException("Window Class registering is missed.");
            case WebView2ShowError.NoCoInitializeCalled: throw new InvalidOperationException("COM Initialize is not called.");
            case WebView2ShowError.WindowSizeAdjustFailed: throw new ArgumentException("Adjusting Window Size is failed.");
            case WebView2ShowError.NoWindowCreated: throw new ArgumentException("No WebView 2 Window created.");
            case WebView2ShowError.NoWebView2ObjectCreated: throw new InvalidOperationException("No WebView 2 Object created.");
            case WebView2ShowError.SoManyWebView2RuntimeDetected: throw new InvalidOperationException("So Many WebView 2 Runtime detected.");
            case WebView2ShowError.WebView2RuntimeVersionMismatch: throw new NotSupportedException("WebView 2 Runtime Version mismatched.");
            case WebView2ShowError.WebView2RuntimeNoInstalled: throw new NotSupportedException("WebView 2 Runtime is no detected.");
            case WebView2ShowError.UserDataFolderAccessDenied: throw new UnauthorizedAccessException("User Data Folder Access Denied.");
            case WebView2ShowError.UnableToStartEdgeRuntime: throw new InvalidOperationException("Unable to Start Edge Runtime.");
            
            default: throw new Exception("Unable to Start Edge Runtime cause Unknown Error Code.");
        }
    }

    public void Close()
    {
        if (_webView == IntPtr.Zero)
            throw new ObjectDisposedException(GetType().Name);
        
        CloseWebView2Window!(_webView);
    }

    public void Navigate(string uri)
    {
        if (_webView == IntPtr.Zero)
            throw new ObjectDisposedException(GetType().Name);

		NavigateWebView2Window!(_webView, uri);
	}

#if UNITY_ENGINE
    [AOT.MonoPInvokeCallback(typeof(WindowCreatedCallbackDelegate))]
#endif
	private static void WindowCreatedCallback(IntPtr webView)
    {
        if (!_webViewDict.TryGetValue(webView, out var webViewObj))
            return;
        
        webViewObj.WindowCreated?.Invoke(webViewObj, EventArgs.Empty);
    }

#if UNITY_ENGINE
	[AOT.MonoPInvokeCallback(typeof(WindowClosingCallbackDelegate))]
#endif
	private static void WindowClosingCallback(IntPtr webView, out int cancel)
    {
        cancel = 0;
        if (!_webViewDict.TryGetValue(webView, out var webViewObj))
            return;

        var cancelEventArgs = new CancelEventArgs(false);
        webViewObj.WindowClosing?.Invoke(webViewObj, cancelEventArgs);

        cancel = cancelEventArgs.Cancel ? 1 : 0;
    }

#if UNITY_ENGINE
	[AOT.MonoPInvokeCallback(typeof(WindowClosedCallbackDelegate))]
#endif
	private static void WindowClosedCallback(IntPtr webView)
    {
        if (!_webViewDict.TryGetValue(webView, out var webViewObj))
            return;
        
        webViewObj.WindowClosed?.Invoke(webViewObj, EventArgs.Empty);
    }

#if UNITY_ENGINE
	[AOT.MonoPInvokeCallback(typeof(NavigatingCallbackDelegate))]
#endif
	private static void NavigatingCallback(IntPtr webView, IntPtr uri, out int cancel)
    {
        cancel = 0;
        if (!_webViewDict.TryGetValue(webView, out var webViewObj))
            return;

        var uriString = uri != IntPtr.Zero ? Marshal.PtrToStringUni(uri) : null;
        var navigatingEventArgs = new NavigatingEventArgs(uriString);
        
        webViewObj.Navigating?.Invoke(webViewObj, navigatingEventArgs);

        cancel = navigatingEventArgs.Cancel ? 1 : 0;
    }

#if UNITY_ENGINE
	[AOT.MonoPInvokeCallback(typeof(NavigatedCallbackDelegate))]
#endif
	private static void NavigatedCallback(IntPtr webView, bool succeeded, IntPtr uri, ushort statusCode)
    {
        if (!_webViewDict.TryGetValue(webView, out var webViewObj))
            return;
        
        var uriString = uri != IntPtr.Zero ? Marshal.PtrToStringUni(uri) : null;
        var navigatedEventArgs = new NavigatedEventArgs(succeeded, uriString, (HttpStatusCode)statusCode);
        
        webViewObj.Navigated?.Invoke(webViewObj, navigatedEventArgs);
    }
}