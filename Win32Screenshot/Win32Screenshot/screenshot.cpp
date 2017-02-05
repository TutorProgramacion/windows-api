#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <wincodec.h>

using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "windowscodecs.lib")

#define BTN_IMAGECAPTURA       120
#define BTN_START_VIDEOCAPTURA 121
#define BTN_STOP_VIDEOCAPTURA  122
#define SCREENSHOT_TIMER       123

HRESULT SaveImage(BYTE* data, BITMAP bmp)
{
    IWICImagingFactory* piFactory = NULL;
    IWICBitmapEncoder *piEncoder = NULL;
    IWICBitmapFrameEncode *piBitmapFrame = NULL;
    IWICStream *piStream = NULL;
    IPropertyBag2 *pPropertybag = NULL;

    static unsigned int frame_count = 0;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory,
        reinterpret_cast<void **>(&piFactory));

    if (SUCCEEDED(hr))
    {
        hr = piFactory->CreateStream(&piStream);
    }

    if (SUCCEEDED(hr))
    {
        wchar_t txt[128];
        wsprintf(txt, L"c:/developer/temp/images/frame_%d.bmp", frame_count++);
        hr = piStream->InitializeFromFilename(txt, GENERIC_WRITE);
    }

    if (SUCCEEDED(hr))
    {
        hr = piFactory->CreateEncoder(GUID_ContainerFormatBmp, NULL, &piEncoder);
    }

    if (SUCCEEDED(hr))
    {
        hr = piEncoder->Initialize(piStream, WICBitmapEncoderNoCache);
    }

    if (SUCCEEDED(hr))
    {
        hr = piEncoder->CreateNewFrame(&piBitmapFrame, &pPropertybag);
    }

    if (SUCCEEDED(hr))
    {
        hr = piBitmapFrame->Initialize(pPropertybag);
    }

    if (SUCCEEDED(hr))
    {
        hr = piBitmapFrame->SetSize(bmp.bmWidth, bmp.bmHeight);
    }

    if (SUCCEEDED(hr))
    {
        WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGR;
        hr = piBitmapFrame->SetPixelFormat(&formatGUID);
    }

    if (SUCCEEDED(hr))
    {
        hr = piBitmapFrame->WritePixels(bmp.bmHeight, bmp.bmWidthBytes, bmp.bmWidthBytes * bmp.bmHeight, data);
    }

    if (SUCCEEDED(hr))
    {
        hr = piBitmapFrame->Commit();
    }

    if (SUCCEEDED(hr))
    {
        hr = piEncoder->Commit();
    }

    if (piBitmapFrame)
        piBitmapFrame->Release();

    if (piEncoder)
        piEncoder->Release();

    if (piStream)
        piStream->Release();

    if (piFactory)
        piFactory->Release();

    return hr;
}

VOID OnScreenshot(HWND hWnd)
{
    HDC hdcScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);
    HDC hdcCompatible = CreateCompatibleDC(hdcScreen);

    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen,
        GetDeviceCaps(hdcScreen, HORZRES),
        GetDeviceCaps(hdcScreen, VERTRES));

    SelectObject(hdcCompatible, hbmScreen);

    BITMAP bmp;
    GetObject(hbmScreen, sizeof(BITMAP), &bmp);

    UINT size_data = bmp.bmWidthBytes * bmp.bmHeight;
    BYTE* data = new BYTE[size_data];

    BitBlt(hdcCompatible, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcScreen, 0, 0, SRCCOPY);

    GetBitmapBits(hbmScreen, size_data, data);

    SaveImage(data, bmp);

    delete[] data;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize COM.
    CoInitialize(NULL);

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("Win32Screenshot");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("Win32Screenshot"),         // window class name
        TEXT("Screenshot Windows API"),  // window caption
        WS_OVERLAPPEDWINDOW,          // window style
        CW_USEDEFAULT,                // initial x position
        CW_USEDEFAULT,                // initial y position
        320,                          // initial x size
        180,                          // initial y size
        NULL,                         // parent window handle
        NULL,                         // window menu handle
        hInstance,                    // program instance handle
        NULL);                        // creation parameters

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    CoUninitialize();

    return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;

    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindow(
            L"BUTTON",                      // Predefined class; Unicode assumed. 
            L"Capturar Imagen (*.bmp)",     // Button text. 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles. 
            10,                       // x position. 
            10,                       // y position. 
            280,                      // Button width.
            30,                       // Button height.
            hWnd,                     // Parent window.
            (HMENU)BTN_IMAGECAPTURA,       // No menu.
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
            NULL);                    // Pointer not needed.

        CreateWindow(
            L"BUTTON",                 // Predefined class; Unicode assumed. 
            L"Iniciar Captura",        // Button text. 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles. 
            10,                        // x position. 
            50,                        // y position. 
            280,                       // Button width.
            30,                        // Button height.
            hWnd,                      // Parent window.
            (HMENU)BTN_START_VIDEOCAPTURA,       // No menu.
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
            NULL);

        CreateWindow(
            L"BUTTON",                 // Predefined class; Unicode assumed. 
            L"Detener Captura",        // Button text. 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles. 
            10,                        // x position. 
            90,                        // y position. 
            280,                       // Button width.
            30,                        // Button height.
            hWnd,                      // Parent window.
            (HMENU)BTN_STOP_VIDEOCAPTURA,       // No menu.
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
            NULL);
    }
    return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case BTN_IMAGECAPTURA:
            OnScreenshot(hWnd);
            break;
        case BTN_START_VIDEOCAPTURA:
            SetTimer(hWnd, SCREENSHOT_TIMER, 1000 / 30, (TIMERPROC)NULL);
            break;
        case BTN_STOP_VIDEOCAPTURA:
            KillTimer(hWnd, SCREENSHOT_TIMER);
            break;
        }
        return 0;

    case WM_TIMER:
        switch (wParam)
        {
        case SCREENSHOT_TIMER:
            OnScreenshot(hWnd);
            break;
        }
        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        //...
        EndPaint(hWnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc