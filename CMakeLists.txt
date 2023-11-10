cmake_minimum_required(VERSION 3.26)

project(EdgeWebView
        VERSION 1.0
        DESCRIPTION "Edge WebView2 Runtime Binder"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(unofficial-webview2 CONFIG REQUIRED)
find_package(wil CONFIG REQUIRED)

if(NOT WIN32)
    message(FATAL_ERROR "This library target to Windows only available.")
endif()
if(NOT MSVC)
    message(FATAL_ERROR "MinGW have not Some Windows Runtime Library component.")
endif()

add_library(EdgeWebView SHARED
        src/DllMain.cpp
        src/EdgeWebView.cpp
        src/Implementation.cpp
        include/EdgeWebView.h
        include/Implementation.h
        app/sample.cpp)
target_include_directories(EdgeWebView PRIVATE unofficial::webview2)
target_include_directories(EdgeWebView PRIVATE include)
target_link_libraries(EdgeWebView PRIVATE unofficial::webview2::webview2 WIL::WIL)
target_compile_definitions(EdgeWebView PRIVATE -DEXPORT_TO_DLL -DWINDOWS -DUNICODE)
if(NOT("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    target_compile_options(EdgeWebView PUBLIC "/MT")
endif()

add_executable(EdgeWebViewSample WIN32 app/sample.cpp app/app.manifest)
target_include_directories(EdgeWebViewSample PRIVATE include)
target_link_libraries(EdgeWebViewSample PRIVATE EdgeWebView)
if(NOT("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
    target_compile_options(EdgeWebViewSample PUBLIC "/MT")
endif()