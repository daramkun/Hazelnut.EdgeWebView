using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Hazelnut.EdgeWebView;

public partial class WebView
{
    private static IntPtr _library;

    private static CreateWebView2WindowDelegate? CreateWebView2Window;
    private static ShowWebView2WindowDelegate? ShowWebView2Window;
    private static CloseWebView2WindowDelegate? CloseWebView2Window;
    private static DestroyWebView2WindowDelegate? DestroyWebView2Window;
    private static NavigateWebView2WindowDelegate? NavigateWebView2Window;

    public static string LibraryPath { get; set; }

    static WebView()
    {
        LibraryPath = "EdgeWebView.dll";
    }

    private static void InitializeModule()
    {
        if (_library != IntPtr.Zero)
            return;
        
        _library = LoadLibrary(LibraryPath);
        if (_library == IntPtr.Zero)
            throw new ArgumentException("EdgeWebView DLL not found or something.");

        var createPtr = GetProcAddress(_library, "CreateWebView2Window");
        var showPtr = GetProcAddress(_library, "ShowWebView2Window");
        var closePtr = GetProcAddress(_library, "CloseWebView2Window");
        var destroyPtr = GetProcAddress(_library, "DestroyWebView2Window");
        var navigatePtr = GetProcAddress(_library, "NavigateWebView2Window");

        if (createPtr == IntPtr.Zero || showPtr == IntPtr.Zero || closePtr == IntPtr.Zero || navigatePtr == IntPtr.Zero)
            throw new ArgumentException("EdgeWebView DLL is invalid.");

        CreateWebView2Window = Marshal.GetDelegateForFunctionPointer<CreateWebView2WindowDelegate>(createPtr);
        ShowWebView2Window = Marshal.GetDelegateForFunctionPointer<ShowWebView2WindowDelegate>(showPtr);
        CloseWebView2Window = Marshal.GetDelegateForFunctionPointer<CloseWebView2WindowDelegate>(closePtr);
        DestroyWebView2Window = Marshal.GetDelegateForFunctionPointer<DestroyWebView2WindowDelegate>(destroyPtr);
        NavigateWebView2Window = Marshal.GetDelegateForFunctionPointer<NavigateWebView2WindowDelegate>(navigatePtr);
    }

    public static void FreeModule()
    {
        FreeLibrary(_library);
        _library = IntPtr.Zero;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    private unsafe struct WebView2Options
    {
        public IntPtr ParentWindowHandle;

        public uint Width;
        public uint Height;
        public int Sizable;

        public int CustomTitle;
        public fixed char Title[128];

        public fixed char UserAgent[256];
        public fixed char UserDataDirectoryPath[1024];

        public WindowCreatedCallbackDelegate WindowCreatedCallback;
        public WindowClosingCallbackDelegate WindowClosingCallback;
        public WindowClosedCallbackDelegate WindowClosedCallback;
        public NavigatingCallbackDelegate NavigatingCallback;
        public NavigatedCallbackDelegate NavigatedCallback;
    }

    private enum WebView2ShowError
    {
	    NoError,

	    InvalidArguments,

	    NoRegisteredWindowClass,
	    NoCoInitializeCalled,
	    WindowSizeAdjustFailed,
	    NoWindowCreated,
	    NoWebView2ObjectCreated,
	    SoManyWebView2RuntimeDetected,
	    WebView2RuntimeVersionMismatch,
	    WebView2RuntimeNoInstalled,
	    UserDataFolderAccessDenied,
	    UnableToStartEdgeRuntime,
    }

	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate IntPtr CreateWebView2WindowDelegate(in WebView2Options options);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate WebView2ShowError ShowWebView2WindowDelegate(IntPtr webView, [MarshalAs(UnmanagedType.LPWStr)] string? uri);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void CloseWebView2WindowDelegate(IntPtr webView);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void DestroyWebView2WindowDelegate(IntPtr webView);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void NavigateWebView2WindowDelegate(IntPtr webView, [MarshalAs(UnmanagedType.LPWStr)] string? uri);

	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void WindowCreatedCallbackDelegate(IntPtr sender);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void WindowClosingCallbackDelegate(IntPtr sender, out int cancel);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void WindowClosedCallbackDelegate(IntPtr sender);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void NavigatingCallbackDelegate(IntPtr sender, IntPtr uri, out int cancel);
	[UnmanagedFunctionPointer(CallingConvention.StdCall)]
	private delegate void NavigatedCallbackDelegate(IntPtr sender, bool succeeded, IntPtr uri, ushort statusCode);
    
#if NETSTANDARD2_0 || NETSTANDARD2_1
    [DllImport("kernel32")]
    private static extern IntPtr LoadLibrary(string libFileName);
    [DllImport("kernel32")]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool FreeLibrary(IntPtr libModule);
    [DllImport("kernel32")]
    private static extern IntPtr GetProcAddress(IntPtr module, [MarshalAs(UnmanagedType.LPStr)] string procName);
#endif
#if NET5_0_OR_GREATER
    private static IntPtr LoadLibrary(string path) => NativeLibrary.Load(path);
    private static void FreeLibrary(IntPtr module) => NativeLibrary.Free(module);
    private static IntPtr GetProcAddress(IntPtr module, string procName) => NativeLibrary.GetExport(module, procName);
#endif
}