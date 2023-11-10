using System.Net;

namespace Hazelnut.EdgeWebView;

public class NavigatedEventArgs : EventArgs
{
    public bool IsSucceeded { get; }
    public string? Uri { get; }
    public HttpStatusCode StatusCode { get; }

    public NavigatedEventArgs(bool isSucceeded, string? uri, HttpStatusCode statusCode)
    {
        IsSucceeded = isSucceeded;
        Uri = uri;
        StatusCode = statusCode;
    }
}