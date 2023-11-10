namespace Hazelnut.EdgeWebView;

public class NavigatingEventArgs : EventArgs
{
    public string? Uri { get; }
    public bool Cancel { get; set; }

    public NavigatingEventArgs(string? uri)
    {
        Uri = uri;
        Cancel = false;
    }
}