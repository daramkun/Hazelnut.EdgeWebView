using Hazelnut.EdgeWebView;
using Sample;
using System.Diagnostics;
using static Sample.PInvoke;

public static class Program
{
	[STAThread]
    private static void Main()
	{
		Environment.CurrentDirectory = AppDomain.CurrentDomain.BaseDirectory;
		WebView.LibraryPath = Path.GetFullPath("../../../../../../cmake-build-debug/EdgeWebView.dll");

		var options = new WebViewOptions();
		options.Width = 1280;
		options.Height = 720;
		options.IsSizable = true;
		options.CustomTitle = "Sample";
		options.UseCustomTitle = true;

		using var webView = new WebView(options);
		webView.WindowCreated += (sender, e) =>
		{
			Debug.WriteLine("Window Created");
		};
		webView.WindowClosing += (sender, e) =>
		{
			Debug.WriteLine("Window Closing");
		};
		webView.WindowClosed += (sender, e) =>
		{
			Debug.WriteLine("Window Closed");
			PostQuitMessage(0);
		};
		webView.Navigating += (sender, e) =>
		{
			Debug.WriteLine("Navigating: {0}", e.Uri);
		};
		webView.Navigated += (sender, e) =>
		{
			Debug.WriteLine("Navigated: {0} ({1}) {2}", e.Uri, e.IsSucceeded, e.StatusCode);
		};

		webView.Show("https://daram.in");

		Message msg = new();
		while (GetMessage(ref msg, IntPtr.Zero, 0, 0))
		{
			TranslateMessage(ref msg);
			DispatchMessage(ref msg);
		}
	}
}