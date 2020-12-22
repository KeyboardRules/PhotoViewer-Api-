// BTLAPI.cpp : Defines the entry point for the application.

#include <Windows.h>
#include "framework.h"
#include "BTLAPI.h"
#include <vector>
#include <string>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

#define MAX_LOADSTRING 100
HWND HwndButton[7],hwndGroup,hwndImageBox,hwndScroll;
HWND hwndEdit;
HICON hIcon[8];

static std::vector<std::string> listImages;
static int counter = 0;
static WCHAR link[100];

int XPic, YPic, XMove , YMove , X , Y ;
int XSize = 300, YSize = 300;
int size=10;
int rotate = 0;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnumChildProc(HWND, LPARAM);
void AdjustLocationOnChangeSize(void);
void AdjustLocationOnDrag(void);
void CreateButton(HWND hWnd,HINSTANCE hInstance);
void CreateIcon(void);
void LoadMyImage(HWND,HDC);
void CreateImage(void);
void open_file(HWND);
void delete_file(HWND);
void remove(HWND);
void save_file(HWND);
void file_properties(HWND);
bool DisplayMessageBox(HWND, LPCWSTR, LPCWSTR);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    
    // TODO: Place code here.

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BTLAPI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BTLAPI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BTLAPI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(ID_ICON));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static RECT rect;
    static HINSTANCE hInstance;
    static unsigned int XBDown, YBDown, XBUp, YBUp;
    HDC hDC = GetWindowDC(NULL);
    switch (message)
    {
    case WM_CREATE:
        hInstance = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);
        
        CreateButton(hWnd,hInstance);
        CreateIcon();

        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_FILE_OPEN:
                open_file(hWnd);
                size = 10;
                SetScrollPos(hwndScroll, SB_CTL, size, true);
                CreateImage();
                AdjustLocationOnChangeSize();
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            case ID_FILE_DELETE:
                if (DisplayMessageBox(hWnd,(LPCWSTR)L"Do you want to delete this image?", (LPCWSTR)L"Delete")) {
                    delete_file(hWnd);
                    size = 10;
                    SetScrollPos(hwndScroll, SB_CTL, size, true);
                    CreateImage();
                    AdjustLocationOnChangeSize();
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                else {
                    MessageBox(hWnd, L"Delete cancel", L"Delete", NULL);
                }
                break;
            case ID_FILE_MAKEACOPY:
                save_file(hWnd);
                break;
            case ID_FILE_PROPERTIES:
                file_properties(hWnd);
                break;
            case 50:
                remove(hWnd);
                CreateImage();
                AdjustLocationOnChangeSize();
                InvalidateRect(hWnd, NULL, TRUE);
            case 70:
                size = 10;
                SetScrollPos(hwndScroll, SB_CTL, size, true);
                AdjustLocationOnChangeSize();
                InvalidateRect(hWnd, NULL, true);
                break;
            case 90:
                if (counter > 0)
                    counter--;
                else
                    counter = listImages.size() - 1;
                size = 10;
                SetScrollPos(hwndScroll, SB_CTL, size, true);
                CreateImage();
                AdjustLocationOnChangeSize();
                InvalidateRect(hWnd, NULL, TRUE);

                break;
            case 110:
                SetWindowPos(hWnd, NULL, 0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), SWP_FRAMECHANGED);
                break;
            case 130:
                if (counter < (listImages.size() - 1))
                    counter++;
                else
                    counter = 0; 
                size = 10;
                SetScrollPos(hwndScroll, SB_CTL, size, true);
                CreateImage();
                AdjustLocationOnChangeSize();
                InvalidateRect(hWnd, NULL, TRUE);

                break;
            case 150:
                if (rotate == 0) rotate = 3;
                else rotate--;
                InvalidateRect(hWnd, NULL, true);
                break;
            case 170:
                rotate = (rotate + 1) % 4;
                InvalidateRect(hWnd, NULL, true);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(ID_ABOUT), hWnd, About);
                break;
            case ID_FILE_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        XBDown = LOWORD(lParam);
        YBDown = HIWORD(lParam);
        break;
    case WM_LBUTTONUP:
        XBUp = LOWORD(lParam);
        YBUp = HIWORD(lParam);
     
        XMove = (XBUp - XBDown);
        YMove = (YBUp - YBDown);

        AdjustLocationOnDrag();
        InvalidateRect(hWnd, NULL, true);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            //code
            LoadMyImage(hWnd,hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }
        break;
    case WM_SIZE:
       GetClientRect(hWnd, &rect);
       EnumChildWindows(hWnd, EnumChildProc, (LPARAM)&rect);
       AdjustLocationOnChangeSize();
       InvalidateRect(hWnd, NULL, true);
       return 0;
    case WM_HSCROLL:
        switch(LOWORD(wParam))
        {
        case SB_PAGELEFT:
            size -= 2;
            break;
        case SB_PAGERIGHT:
            size += 2;
            break;
        case SB_LINELEFT:
            if(size>10) size -=4;
            break;
        case SB_LINERIGHT:
            if(size<50) size += 4;
            break;
        case SB_LEFT:
            size = 10;
            break;
        case SB_RIGHT:
            size = 40;
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            size = HIWORD(wParam);
            break;
        default:
            break;
        }
        SetScrollPos(hwndScroll, SB_CTL, size, true);
        AdjustLocationOnChangeSize();
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = 800;
        mmi->ptMinTrackSize.y = 400;
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
{
    int  idChild;
    idChild = GetWindowLong(hwndChild, GWL_ID);
    LPRECT rcParent;
    rcParent = (LPRECT)lParam;
    XPic = ((rcParent->right - rcParent->left) / 2);
    YPic = ((rcParent->bottom - rcParent->top) / 2);
    // Size and position the child window.  
    if (idChild == 0) {
        MoveWindow(hwndChild,
            -rcParent->left,
            rcParent->bottom - 100,
            rcParent->right - rcParent->left
            , 100,
            TRUE);
    }
    else if (idChild == 1) {
        MoveWindow(hwndChild,
            (rcParent->right-rcParent->left)/2-390,
            rcParent->bottom - 70,
            30, 30,
            TRUE);
    }
    else if (idChild == 2) {
        MoveWindow(hwndChild,
            (rcParent->right - rcParent->left) / 2 - 350,
            rcParent->bottom - 70,
            150, 20,
            TRUE);
    }
    else if(idChild!=3&&idChild!=4) {
        MoveWindow(hwndChild,
            (rcParent->right-rcParent->left)/2-360+idChild*4,
            rcParent->bottom -75,
            50,50,
            TRUE);
    }
    // Make sure the child window is visible. 

    ShowWindow(hwndChild, SW_SHOW);

    return TRUE;
}
void CreateButton(HWND hWnd,HINSTANCE hInstance) {
    hwndGroup = CreateWindow(TEXT("Static"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_FLAT | BS_OWNERDRAW | BS_GROUPBOX, 0, 0, 0,0, hWnd, (HMENU)0, hInstance, NULL);
    //Group box
    for (int i = 0; i < 7; i++) {
        HwndButton[i] = CreateWindow(TEXT("button"), TEXT(""),
            BS_ICON | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | BS_FLAT, 0, 0, 0,0, hWnd, (HMENU)(50 + i * 20), hInstance, NULL);
    }
    hwndScroll = CreateWindow(TEXT("scrollbar"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | SBS_HORZ, 0, 0, 0, 0, hWnd,(HMENU)2, hInstance, NULL);
    SetScrollRange(hwndScroll, SB_CTL, 10, 50, FALSE);
    SetScrollPos(hwndScroll, SB_CTL, 10, FALSE);

    hwndImageBox = CreateWindow(TEXT("Static"), TEXT(""),
        SS_ICON | WS_CHILD | WS_VISIBLE|SS_REALSIZEIMAGE,
        0, 0, 0,0, hWnd, (HMENU)1, hInstance, NULL);
}
void CreateIcon() {
    hIcon[0] = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_DELETE));
    hIcon[1] = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_DEFAULT));
    hIcon[2] = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_PREVIOUS));
    hIcon[3] = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_FULL_SIZE));
    hIcon[4] = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_NEXT));
    hIcon[5] = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_ROTATE_LEFT));
    hIcon[6] = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_ROTATE_RIGHT));
    hIcon[7]= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON_ZOOM));
    for (int i = 0; i < 7; i++) {
        SendMessage(HwndButton[i], BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon[i]);
    }
    SendMessage(hwndImageBox, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon[7]);
}
void AdjustLocationOnDrag() {
    X += XMove;
    Y += YMove;
}
void AdjustLocationOnChangeSize() {
    X = XPic-XSize * size / 20;
    Y =YPic- YSize * size / 20;
}
void CreateImage() {
   // SetWindowText(hwndEdit, link);
    if (!listImages.empty()) {
        std::wstring ws(listImages[counter].begin(), listImages[counter].end());
        wcscpy_s(link, ws.c_str());
        Image img(link);
        rotate = 0;
        XSize = img.GetWidth();
        YSize = img.GetHeight();
    }
    else {
        wcscpy_s(link,L"");
    }
}
//L"C:\\Users\\Admin\\Desktop\\TestArea\\Rotate-Right.bmp"
void LoadMyImage(HWND hwnd,HDC hdc) {
    Graphics graphics(hdc);
    Image img(link);

    // Draw the image.
    if (rotate == 0) {
        img.RotateFlip(RotateNoneFlipNone);
    }
    if (rotate == 1) {
        img.RotateFlip(Rotate90FlipNone);
    }
    if (rotate == 2) {
        img.RotateFlip(Rotate180FlipNone);
    }
    if (rotate == 3) {
        img.RotateFlip(Rotate270FlipNone);
    }
    graphics.DrawImage(&img, X, Y, XSize*size/10, YSize*size/10);
}
void open_file(HWND hWnd) {
    OPENFILENAME openFile;

    WCHAR file_name[100];

    ZeroMemory(&openFile, sizeof(OPENFILENAME));

    openFile.lStructSize = sizeof(OPENFILENAME);
    openFile.hwndOwner = hWnd;
    openFile.lpstrFile = file_name;
    openFile.lpstrFile[0] = '\0';
    openFile.nMaxFile = 100;
    openFile.lpstrFilter = L"JPG Files\0*.jpg\0PNG Files\0*.png\0BMP Files\0*.bmp\0ICO files\0*.ico\0";
    openFile.nFilterIndex = 0;

    if (GetOpenFileName(&openFile)) {
        std::wstring ws(file_name);
        std::string std(ws.begin(), ws.end());
        listImages.push_back(std);
        counter = listImages.size() - 1;
    }
}
void save_file(HWND hWnd) {
    OPENFILENAME openFile;

    WCHAR file_name[100];

    ZeroMemory(&openFile, sizeof(OPENFILENAME));

    openFile.lStructSize = sizeof(OPENFILENAME);
    openFile.hwndOwner = hWnd;
    openFile.lpstrFile = file_name;
    openFile.lpstrFile[0] = '\0';
    openFile.nMaxFile = 100;
    openFile.lpstrFilter = L"JPG Files\0*.jpg\0PNG Files\0*.png\0BMP Files\0*.bmp\0ICO files\0*.ico\0";
    openFile.nFilterIndex = 0;

    if (GetSaveFileName(&openFile)) {
        Image img(link);
        CLSID pngClsid;
        GetEncoderClsid(L"image/png", &pngClsid);
        img.Save(file_name, &pngClsid, NULL);
    }
}
void delete_file(HWND hWnd) {
    if (!listImages.empty()) {
        std::wstring ws(listImages[counter].begin(), listImages[counter].end());
        if (DeleteFileW(ws.c_str())) {
            MessageBox(hWnd, L"Successfully delete file", L"Delete", NULL);
            listImages.erase(listImages.begin() + counter);
            if (counter > listImages.size() - 1) {
                counter = listImages.size() - 1;
            }
        }
        else {
            MessageBox(hWnd, L"Delete Failed", L"Delete", NULL);
        }
    }
}
void remove(HWND hWnd) {
    if (!listImages.empty()) {
        listImages.erase(listImages.begin() + counter);
        if (counter > listImages.size() - 1) {
            counter = listImages.size() - 1;
        }
    }
}

void file_properties(HWND hWnd) {
    SHELLEXECUTEINFO info = { 0 };

    info.cbSize = sizeof info;
    info.lpFile = link;
    info.nShow = SW_SHOW;
    info.fMask = SEE_MASK_INVOKEIDLIST;
    info.lpVerb = L"properties";

    ShellExecuteEx(&info);
}
bool DisplayMessageBox(HWND hWnd, LPCWSTR message,LPCWSTR title)
{
    int msgboxID = MessageBox(
        NULL,
        message,
        title,
        MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2
        );

    switch (msgboxID)
    {
    case IDYES:
        return true;
    case IDNO:
        return false;
    default:
        return false;
    }
    return false;
}
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

