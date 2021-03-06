// TotalCommander.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TotalCommander.h"
#include <windowsx.h>
#include <commctrl.h>

#define MAX_LOADSTRING 100
#define TYPE 1
#define SIZE 2
#define DATEMODIFIED 3

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnDestroy(HWND hwnd);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);
void _GetInfoFile(WIN32_FIND_DATA data, int option);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TOTALCOMMANDER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOTALCOMMANDER));

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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOTALCOMMANDER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TOTALCOMMANDER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
      CW_USEDEFAULT, 0, 900, 600, nullptr, nullptr, hInstance, nullptr);

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

    switch (message)
    {
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

    //case WM_PAINT:
    //    {
    //        PAINTSTRUCT ps;
    //        HDC hdc = BeginPaint(hWnd, &ps);
    //        // TODO: Add any drawing code that uses hdc here...
    //        EndPaint(hWnd, &ps);
    //    }
    //    break;
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

// ================ Init Func ===================

HWND g_hTreeView, g_hListView;
HTREEITEM g_hThisPC;

bool loaded = false;


BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
	// lấy font hệ thống
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	g_hTreeView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
		0, 0, 300, 540, hwnd,
		(HMENU)IDC_TREEVIEW, lpCreateStruct->hInstance, NULL);
	SetWindowFont(g_hTreeView, hFont, TRUE);

	g_hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT,
		300, 0, 600, 540, hwnd,
		(HMENU)IDC_LISTVIEW, lpCreateStruct->hInstance, NULL);
	SetWindowFont(g_hListView, hFont, TRUE);

	// Thêm nút gốc This PC vào TreeView
	TV_INSERTSTRUCT tvInsert;	// Cấu trúc dữ liệu dùng để chèn một node vào TreeView
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN; // Chèn thêm text và dữ liệu bên dưới - param
	tvInsert.hParent = NULL;			// Không có cha, hiển nhiên!
	tvInsert.hInsertAfter = TVI_ROOT;	// Chèn ngay đầu
	tvInsert.item.pszText = (LPWSTR)L"This PC"; // Text hiển thị của node này
	tvInsert.item.lParam = (LPARAM)L"Root";	 // Dữ liệu tại node để xử lí
	tvInsert.item.cChildren = 1; // Nếu có nút con sẽ tự động được vẽ nút để bấm vào sổ xuống
	g_hThisPC = TreeView_InsertItem(g_hTreeView, &tvInsert);

	// Load drive
	const int BUFFER_SIZE = 26 * 4; // 26 kí tự từ A - Z,

	WCHAR drives[BUFFER_SIZE];
	GetLogicalDriveStrings(BUFFER_SIZE, drives);

	WCHAR* name = drives; // Con trỏ trỏ đến vị trí đầu chuỗi
	do
	{
		tvInsert.hParent = g_hThisPC;
		tvInsert.hInsertAfter = TVI_LAST; // Cứ chèn từ trên xuống dưới
		// chỗ này cần get ra tên của ỗ đĩa
		tvInsert.item.pszText = name;
		int len = wcslen(name);
		WCHAR* path = new WCHAR[len + 1]; // QUAN TRỌNG: RÒ RỈ BỘ NHỚ Ở ĐÂY
		wcscpy_s(path, len + 1, name);
		tvInsert.item.lParam = (LPARAM)path;
		tvInsert.item.cChildren = 1;
		TreeView_InsertItem(g_hTreeView, &tvInsert);
		//delete[]path;
		int i = 0;
		while (name[i] != '\0') i++; // Trong khi chưa gặp kí tự \0 thì tăng 1
		// Dừng lại thì i đang ở kí tự \0
		name = &name[i + 1]; // Bỏ qua kí tự \0 hiện tại và đi tới kí tự kế
	} while (name[0] != '\0'); // Chưa gặp 2 \0 liên tiếp

	TreeView_Expand(g_hTreeView, g_hThisPC, TVE_EXPAND);
	TreeView_SelectItem(g_hTreeView, g_hThisPC);
	loaded = true;

	// LIST VIEW

	// Thông tin cột cần chèn
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;

	// Cột thứ nhất
	col.cx = 200;
	col.pszText = (LPWSTR)L"Name";
	ListView_InsertColumn(g_hListView, 0, &col);

	// Cột thứ hai
	col.cx = 120;
	col.pszText = (LPWSTR)L"Type";
	ListView_InsertColumn(g_hListView, 1, &col);

	// Cột thứ ba
	col.cx = 80;
	col.pszText = (LPWSTR)L"Size";
	ListView_InsertColumn(g_hListView, 2, &col);

	// Cột thứ tư
	col.cx = 200;
	col.pszText = (LPWSTR)L"Last modified";
	ListView_InsertColumn(g_hListView, 3, &col);
	
	return true;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

}
WCHAR tmpGetInfoFile[30];
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm) {
	if (loaded) { // Đã nạp xong dữ liệu ban đầu rồi
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;
		HTREEITEM node;
		switch (pnm->code)
		{
		case TVN_ITEMEXPANDING:
			node = lpnmTree->itemNew.hItem;
			if (node != g_hThisPC) { // Không phải node gốc
				TVITEMEX tv;
				tv.mask = TVIF_PARAM;
				tv.hItem = node;
				
				TreeView_GetItem(g_hTreeView, &tv);
				
				// Tạo đường dẫn để tìm
				const int BUFFER_SIZE = 260;
				WCHAR buffer[BUFFER_SIZE];
				wcscpy_s(buffer, BUFFER_SIZE, (LPWSTR)tv.lParam);
				wcscat_s(buffer, L"\\*");
				// Duyệt qua đường dẫn lấy các thư mục con và thêm vào
				if (wcsncmp((LPWSTR)tv.lParam,L"(LoadOK)",8) != 0) {
					WIN32_FIND_DATA data;
					HANDLE hFile = FindFirstFile(buffer, &data); // Tìm tập tin đầu tiên
					do {
						if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
							!(data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
							!(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
							(wcscmp(data.cFileName, L".") != 0) &&
							(wcscmp(data.cFileName, L"..") != 0))
						{
							TV_INSERTSTRUCT tvInsert;
							tvInsert.hParent = node;
							tvInsert.hInsertAfter = TVI_LAST;
							tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
							tvInsert.item.pszText = data.cFileName;
							tvInsert.item.cChildren = 1;
							// Tạo đường dẫn đầy đủ
							int len = wcslen((LPWSTR)tv.lParam)
								+ wcslen(L"\\") + wcslen(data.cFileName) + 1;
							WCHAR * path = new WCHAR[len];
							wcscpy_s(path, len, (LPWSTR)tv.lParam);
							wcscat_s(path, len, L"\\");
							wcscat_s(path, len, data.cFileName);
							tvInsert.item.lParam = (LPARAM)path; // QUAN TRỌNG: RÒ RỈ BỘ NHỚ!!
							TreeView_InsertItem(g_hTreeView, &tvInsert);
						}
					} while (FindNextFile(hFile, &data)); // Cho đến khi không còn tập tin kế
					FindClose(hFile);

					int lenLoadedPath = wcslen((LPWSTR)tv.lParam) + 9;
					WCHAR* LoadedPath = new WCHAR[lenLoadedPath];
					wcscpy_s(LoadedPath, lenLoadedPath, L"(LoadOK)");
					wcscat_s(LoadedPath, lenLoadedPath, (LPWSTR)tv.lParam);
					wcscpy_s((LPWSTR)tv.lParam, lenLoadedPath, LoadedPath);
					
					delete LoadedPath;
				}
				//MessageBox(0, (LPWSTR)tv.lParam, 0, 0);
				TreeView_SetItem(g_hTreeView, &tv);
			}
			break;

		case TVN_SELCHANGED:
			// Lấy node đang được select
			node = TreeView_GetNextItem(g_hTreeView, NULL, TVGN_CARET);
			if (node != g_hThisPC) {
				//  Lấy đường dẫn từ lParam của node
				TVITEMEX tv;
				tv.mask = TVIF_PARAM;
				tv.hItem = node;
				TreeView_GetItem(g_hTreeView, &tv);

				int lenLoadedPath = wcslen((LPWSTR)tv.lParam) + 1;
				WCHAR* GetLoadedPath = new WCHAR[lenLoadedPath];
				wcscpy_s(GetLoadedPath, lenLoadedPath, (LPWSTR)tv.lParam);
				if (wcsncmp((LPWSTR)tv.lParam, L"(LoadOK)", 8) == 0)
				{
					wcscpy_s(GetLoadedPath, lenLoadedPath, (LPWSTR)tv.lParam + 8);
				}

				// Xóa trống ListView lại từ đầu
				ListView_DeleteAllItems(g_hListView);

				// Tạo đường dẫn để tìm
				const int BUFFER_SIZE = 260;
				WCHAR buffer[BUFFER_SIZE];
				wcscpy_s(buffer, BUFFER_SIZE, GetLoadedPath);
				wcscat_s(buffer, L"\\*");

				// Duyệt qua đường dẫn lấy các thư mục con và thêm vào
				WIN32_FIND_DATA data;
				HANDLE hFile = FindFirstFile(buffer, &data); // Tìm tập tin đầu tiên
				int itemIndex = 0; // Dòng đầu tiên

				do {
					if (!(data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
						!(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
						(wcscmp(data.cFileName, L".") != 0) &&
						(wcscmp(data.cFileName, L"..") != 0))
					{
						LV_ITEM row;
						row.mask = LVIF_TEXT | LVIF_PARAM;
						row.iItem = itemIndex;
						// Cột 1 - tên
						row.iSubItem = 0;
						row.pszText = data.cFileName;
						row.lParam = (LPARAM)data.cFileName;
						// Kết hợp đường dẫn với tên thư mục là sẽ ra path ngay
						ListView_InsertItem(g_hListView, &row);

						// Cột 2 - Loại
						
						_GetInfoFile(data, TYPE);
						ListView_SetItemText(g_hListView, itemIndex, 1, (LPWSTR)tmpGetInfoFile);
						

						// Cột 3 - Kích thước
						_GetInfoFile(data, SIZE);
						ListView_SetItemText(g_hListView, itemIndex, 2, (LPWSTR)tmpGetInfoFile);

						// Cột 4 - Ngày sửa đổi
						_GetInfoFile(data, DATEMODIFIED);
						ListView_SetItemText(g_hListView, itemIndex, 3, (LPWSTR)tmpGetInfoFile);

						itemIndex++; // Qua dòng kế
					}
				} while (FindNextFile(hFile, &data)); // Cho đến khi không còn tập tin kế

				FindClose(hFile);

				delete[] GetLoadedPath;
			}
			break;
		}
	}
	return 0;
}

void OnDestroy(HWND hwnd) {
	PostQuitMessage(0);
}

void _GetInfoFile(WIN32_FIND_DATA data, int option)
{
	switch (option)
	{
	case TYPE:	// type
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			wcscpy_s(tmpGetInfoFile, 50, L"Folder");
		}
		else
		{
			wcscpy_s(tmpGetInfoFile, 50, L"File");
		}
		break;
	case SIZE:
		wcscpy_s(tmpGetInfoFile, 20, L"");
		if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			LARGE_INTEGER size;
			
			size.HighPart = data.nFileSizeHigh;
			size.LowPart = data.nFileSizeLow;
			int count = 0;
			
			double dKQ = 0;
			dKQ = (double)size.QuadPart;
			if (dKQ <= 1024)
			{
				dKQ = 1;
				count = 1;
			}
			else
			{
				while (dKQ > 1024)
				{
					dKQ /= 1024;
					count++;
				}
			}

			swprintf_s(tmpGetInfoFile ,50 , L"%.2f", dKQ);

			switch(count)
			{
			case 0:
				wcscat_s(tmpGetInfoFile, 50, L" Bytes");
				break;
			case 1:
				wcscat_s(tmpGetInfoFile, 50, L" KB");
				break;
			case 2:
				wcscat_s(tmpGetInfoFile, 50, L" MB");
				break;
			case 3:
				wcscat_s(tmpGetInfoFile, 50, L" GB");
				break;
			case 4:
				wcscat_s(tmpGetInfoFile, 50, L" TB");
				break;
			}
		}
		break;
	case DATEMODIFIED:
			SYSTEMTIME stUTC, stLocal;
			FileTimeToSystemTime(&data.ftLastAccessTime, &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			WORD wHour = stLocal.wHour;
			WCHAR TypeOfHour[3] = L"AM"; // AM or PM
			if (stLocal.wHour > 12)
			{
				wHour = stLocal.wHour - 12;
				wcscpy_s(TypeOfHour, 3, L"PM");
			}
			wsprintf(tmpGetInfoFile, L"%02d/%02d/%d %02d:%02d %s\n", stLocal.wDay, stLocal.wMonth, stLocal.wYear,
				wHour, stLocal.wMinute, TypeOfHour);
		break;
	}
}