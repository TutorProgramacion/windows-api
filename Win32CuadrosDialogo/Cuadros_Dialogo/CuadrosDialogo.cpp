#include <windows.h>

#define BTN_COLOR 1001
#define BTN_FONT  1002
#define BTN_SAVE  1003
#define BTN_OPEN  1004

HBRUSH   hbrush = NULL;
HFONT    hfont = NULL;
COLORREF tcolor = NULL;

VOID OnPaint(HDC hdc)
{
    SelectObject(hdc, hbrush);
    Rectangle(hdc, 10, 50, 150 + 10, 100);

    SelectObject(hdc, hfont);
    SetTextColor(hdc, tcolor);
    TextOut(hdc, 150 + 20, 50, L"Nueva Fuente", 13);
}

VOID OnChooseColor(HWND hwnd)
{
    CHOOSECOLOR cc;                 // common dialog box structure 
    COLORREF acrCustClr[16]; // array of custom colors    

    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hwnd;                   // ventana principal
    cc.lpCustColors = (LPDWORD)acrCustClr; // colores personalidados
    cc.rgbResult = RGB(0, 255, 0);         // color inicial
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;   // mostrar el cuadro completo y seleccionar el color inicial

    if (ChooseColor(&cc) == TRUE)
    {
        // crear una brocha con el color seleccionado
        hbrush = CreateSolidBrush(cc.rgbResult);

        // volver a pintar la ventana
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

VOID OnChooseFont(HWND hwnd)
{
    CHOOSEFONT cf;            // common dialog box structure
    LOGFONT lf;               // logical font structure

    // Initialize CHOOSEFONT
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.rgbColors = RGB(0, 0, 255);
    cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

    if (ChooseFont(&cf) == TRUE)
    {
        // crear la fuente a partir de la seleccion
        hfont = CreateFontIndirect(cf.lpLogFont);

        // guargar el color de texto
        tcolor = cf.rgbColors;
        
        // volver a pintar la ventana
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

VOID OnOpenFile(HWND hwnd)
{
    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[260];      // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn) == TRUE) {
        MessageBox(hwnd, szFile, L"Abrir archivo:",  MB_OK);
    }
}

VOID OnSaveFile(HWND hwnd)
{
    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[260];      // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 2;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 
    if (GetSaveFileName(&ofn) == TRUE) {
        MessageBox(hwnd, szFile, L"Abrir archivo:", MB_OK);
    }
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("DialogBoxes");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("DialogBoxes"),         // window class name
        TEXT("Cuadros de Dialogo"),  // window caption
        WS_OVERLAPPEDWINDOW,         // window style
        CW_USEDEFAULT,               // initial x position
        CW_USEDEFAULT,               // initial y position
        345,                         // initial x size
        220,                         // initial y size
        NULL,                        // parent window handle
        NULL,                        // window menu handle
        hInstance,                   // program instance handle
        NULL);                       // creation parameters

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;

    switch (message)
    {
    case WM_CREATE:

        CreateWindowW(L"button", L"Buscar Color...",
            WS_VISIBLE | WS_CHILD,
            10, 10, 150, 25,
            hWnd, (HMENU)BTN_COLOR, NULL, NULL);

        CreateWindowW(L"button", L"Cambiar Fuente...",
            WS_VISIBLE | WS_CHILD,
            150 + 20, 10, 150, 25,
            hWnd, (HMENU)BTN_FONT, NULL, NULL);

        CreateWindowW(L"button", L"Abrir...",
            WS_VISIBLE | WS_CHILD,
            10, 110, 310, 25,
            hWnd, (HMENU)BTN_OPEN, NULL, NULL);

        CreateWindowW(L"button", L"Guardar...",
            WS_VISIBLE | WS_CHILD,
            10, 140, 310, 25,
            hWnd, (HMENU)BTN_SAVE, NULL, NULL);

        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case BTN_COLOR:
            OnChooseColor(hWnd);
            break;
        case BTN_FONT:
            OnChooseFont(hWnd);
            break;
        case BTN_OPEN:
            OnOpenFile(hWnd);
            break;
        case BTN_SAVE:
            OnSaveFile(hWnd);
            break;
        default:
            break;
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc