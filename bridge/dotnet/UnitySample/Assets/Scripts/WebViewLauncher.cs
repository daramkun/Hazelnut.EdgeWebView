using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using Hazelnut.EdgeWebView;
using TMPro;
using UnityEngine;

public class WebViewLauncher : MonoBehaviour
{
    private WebView _webView;
    
    [SerializeField]
    private TMP_InputField _inputField;

    protected void Awake()
    {
        WebView.LibraryPath = Path.Combine(Application.dataPath, "Plugins/x86_64/EdgeWebView.dll");
    }
    
    public void OnClickButton()
    {
        if (_webView == null)
        {
            var options = new WebViewOptions();
            options.ParentWindowHandle = UnityUtil.WindowHandle;
            options.UseCustomTitle = true;
            options.CustomTitle = "Edge WebView in Unity";
            options.IsSizable = true;
            options.UserDataPath = Path.Combine(Application.persistentDataPath, "WebView");
            
            _webView = new WebView(options);
            _webView.WindowCreated += (sender, e) =>
            {
                Debug.Log("Window Created");
            };
            _webView.WindowClosing += (sender, e) =>
            {
                Debug.Log("Window Closing");
            };
            _webView.WindowClosed += (sender, e) =>
            {
                Debug.Log("Window Closed");
            };
            _webView.Navigating += (sender, e) =>
            {
                Debug.Log("Navigating");
            };
            _webView.Navigated += (sender, e) =>
            {
                Debug.Log("Navigated");
            };
        }

        _webView.Show(_inputField.text);
    }

    protected void OnApplicationQuit()
    {
        _webView.Dispose();
        _webView = null;

        WebView.FreeModule();
    }
}
