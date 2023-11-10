using System.Runtime.InteropServices;

namespace Hazelnut.EdgeWebView;

#if UNITY_ENGINE
public static class UnityUtil
{
    [DllImport("kernel32", SetLastError = true)]
    private static extern uint GetCurrentThreadId();
    [DllImport("user32", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern unsafe int GetClassName(IntPtr hWnd, char* str, int maxCount);
    [DllImport("user32", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool EnumThreadWindows(uint threadId, EnumWindowsProc enumFunc, IntPtr lParam);

    [return: MarshalAs(UnmanagedType.Bool)]
    private delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    private static IntPtr _hWnd;

    public static unsafe IntPtr WindowHandle
    {
        get
        {
            if (_hWnd != IntPtr.Zero)
                return _hWnd;

            var threadId = GetCurrentThreadId();
            EnumThreadWindows(threadId, InternalEnumThreadWindows, IntPtr.Zero);

            return _hWnd;

            [AOT.MonoPInvokeCallback(typeof(Action<IntPtr, IntPtr>))]
            static bool InternalEnumThreadWindows(IntPtr hWnd, IntPtr lParam)
            {
                var classText = stackalloc char[512];
                var classTextLength = GetClassName(hWnd, classText, 512);

                var classTextStr = new string(classText, 0, classTextLength);
                
                if (classTextStr is "UnityWndClass" or "UnityContainerWndClass")
                {
                    _hWnd = hWnd;
                    return false;
                }

                return true;
            }
        }
    }
}
#endif