using System.Drawing;
using System.Runtime.InteropServices;

namespace Sample;

public static class PInvoke
{
    [DllImport("user32")]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static extern bool GetMessage(ref Message message, IntPtr hWnd, int min, int max);

    [DllImport("user32", CharSet = CharSet.Unicode)]
    public static extern void TranslateMessage(ref Message message);

    [DllImport("user32")]
    public static extern void DispatchMessage(ref Message message);

    [DllImport("user32")]
    public static extern void PostQuitMessage(int exitCode);
    
    [StructLayout(LayoutKind.Sequential)]
    public struct Message
    {
        public IntPtr WindowHandle;
        public uint WindowMessage;
        public IntPtr wParam;
        public IntPtr lParam;
        public int Time;
        public Point Pointer;
        private int Private;
    }
}