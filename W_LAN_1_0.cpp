#include <windows.h>
#include <sstream>
#include "computer.h"
#include "Helper_W_LAN.h"
#include "dlgtab.h"

#define ID_LIST 1
#define ID_BTN_1 2
#define ID_BTN_2 2
#define ID_BTN_3 3
#define ID_BTN_4 4
#define ID_BTN_5 5


LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM); // функция обработки сообщений окна
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM); //функция обработки сообщений диалога



static SOCKET S;
WSADATA Ws;

/*---------------- 
Стартовая функция
-----------------*/
int  PASCAL  WinMain(HINSTANCE  hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd; // дескриптор окна
	MSG msg;   // структура сообщения
	WNDCLASS w; // структура класса окна
	RECT desctopRect;
	GetWindowRect(GetDesktopWindow(), &desctopRect);
	COLORREF backGroundColor = RGB(23, 72, 104);
	memset(&w, 0, sizeof(WNDCLASS)); // очистка памяти для структуры
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = WndProc;
	w.hInstance = hInstance;
	w.hbrBackground = CreateSolidBrush(backGroundColor);
	w.lpszClassName = "MyClass";
	RegisterClass(&w); // регистрация класса окна
	 // Создание окна
	hwnd = CreateWindow("MyClass", "W_LAN Cаукин Д. О.",
		WS_OVERLAPPEDWINDOW,
		desctopRect.left, desctopRect.top, desctopRect.right, desctopRect.bottom - 40,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow); // отображение окна
	UpdateWindow(hwnd);         // перерисовка окна

	// Цикл обработки сообщений
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

/*-------------------------
Функция обработки сообщений
--------------------------*/
LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam)
{
	static LRESULT currVecCompElem;
	std::vector <computer> vecComp;
	static char outputBuff[1000] = "";

	static HINSTANCE hInst;
	PAINTSTRUCT ps;
	HDC hdc;

	static HWND hBtn1, hBtn2, hBtn3, hBtn4, hBtn5; // дескрипторы кнопок
	static HWND hEdit; // дескриптор поля ввода
	static HWND hList1, hInfoWindow; // дескрипторы списка и инф. окна
	char* IPstr = new char[17];
	RECT rekt;
	int editLength;

	if (FAILED(WSAStartup(MAKEWORD(1, 1), &Ws)))
	{

	}
	//создание сокета
	if (INVALID_SOCKET == (S = socket(AF_INET, SOCK_DGRAM, 0)))
	{

	}

	//Заполняем Вектор элементами
	getObjectsFromFile(vecComp);
	vecComp.shrink_to_fit();
	sortVecComp(vecComp);
	int vecSize = vecComp.size();

	switch (Message)
	{
	case WM_CREATE: // сообщение создания окна
		hInst = ((LPCREATESTRUCT)lparam)->hInstance; // дескриптор приложения
		GetWindowRect(hwnd, &rekt);
		//список компьютеров
		hList1 = CreateWindow("listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_WANTKEYBOARDINPUT |
			LBS_USETABSTOPS, 10, 10, rekt.right - 200, rekt.bottom - 200,
			hwnd, (HMENU)ID_LIST, hInst, NULL);
		ShowWindow(hList1, SW_SHOWNORMAL);
		// статические информационные окна
		hInfoWindow = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_AUTOVSCROLL | 
			ES_READONLY | ES_MULTILINE, 10, rekt.bottom - 150, rekt.right - 50, 140, hwnd, 0, hInst, NULL);
		ShowWindow(hInfoWindow, SW_SHOWNORMAL);
		hEdit = CreateWindow("edit", " ", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
			rekt.right - 170, 10, 120, 30, hwnd, 0, hInst, NULL);
		ShowWindow(hEdit, SW_SHOWNORMAL);
		//кнопка 1
		hBtn1 = CreateWindow("button", "Найти", WS_CHILD | WS_VISIBLE | WS_BORDER,
			rekt.right - 170, 50, 120, 30, hwnd, (HMENU)ID_BTN_1, hInst, NULL);
		ShowWindow(hBtn1, SW_SHOWNORMAL);
		//кнопка 2
		hBtn2 = CreateWindow("button", "Включить", WS_CHILD | WS_VISIBLE | WS_BORDER,
			rekt.right - 170, 90, 120, 30, hwnd, (HMENU)ID_BTN_2, hInst, NULL);
		ShowWindow(hBtn2, SW_SHOWNORMAL);
		//кнопка 3
		hBtn3 = CreateWindow("button", "Выключить", WS_CHILD | WS_VISIBLE | WS_BORDER,
			rekt.right - 170, 130, 120, 30, hwnd, (HMENU)ID_BTN_3, hInst, NULL);
		ShowWindow(hBtn3, SW_SHOWNORMAL);
		//кнопка 4
		hBtn4 = CreateWindow("button", "PING", WS_CHILD | WS_VISIBLE | WS_BORDER,
			rekt.right - 170, 170, 120, 30, hwnd, (HMENU)ID_BTN_4, hInst, NULL);
		ShowWindow(hBtn4, SW_SHOWNORMAL);
		//кнопка 5
		hBtn5 = CreateWindow("button", "Выключить ко времени", WS_CHILD | WS_VISIBLE | WS_BORDER |
			BS_MULTILINE, rekt.right - 170, 210, 120, 50, hwnd, (HMENU)ID_BTN_4, hInst, NULL);
		ShowWindow(hBtn4, SW_SHOWNORMAL);

		//заполняем список элементами
		SendMessage(hList1, WM_SETREDRAW, FALSE, 0L);
		for (int i = 0; i < vecSize; i++)
		{
			std::vector <char> stringOfList = makeString(vecComp[i]);
			SendMessage(hList1, LB_ADDSTRING, 0, (LPARAM)&stringOfList[0]);
		}
		SendMessage(hList1, WM_SETREDRAW, TRUE, 0L);
		InvalidateRect(hList1, NULL, TRUE);

		break;
	case WM_COMMAND:  // сообщение о команде
		if (lparam == (LPARAM)hBtn1)    // если нажали на кнопку 1
		{
			editLength = GetWindowText(hEdit, IPstr, 17);
			if (editLength != 0)
			{
				if (0 == PING(&IPstr[1])) //если компьютер включен
				{
					computer C(&IPstr[1], S, Ws);
					if (addNewItemInFile(C)) //если компьютера нет в списке
					{
						vecComp.push_back(C);
						std::vector <char> stringOfList = makeString(C);
						SendMessage(hList1, LB_ADDSTRING, 0, (LPARAM)&stringOfList[0]);
					}
					else
						MessageBox(hwnd, "Элемент уже существует", "Ошибка", MB_OK);
				}
				else
				{
					MessageBox(hwnd, "Запрашиваемый удалённый компьютер должен быть включен", "Ошибка", MB_OK);
				}
			}
		}
		//кнопку 2
		if (lparam == (LPARAM)hBtn2)
		{
			currVecCompElem = SendMessage(hList1, LB_GETCARETINDEX, 0, 0L);
			int res = vecComp[currVecCompElem].wake_up(S, Ws);
			if (res != 0)
				MessageBox(hwnd, "WAKE_UP ERROR", "Error", MB_OK);
			SendMessageA(hList1, WM_CHILDACTIVATE, 0, 0L);

			//фомируем строку
			strcat(outputBuff, "включить ");
			strcat(outputBuff, vecComp[currVecCompElem].Info.Name);
			strcat(outputBuff, "\r\n");
			SetWindowTextA(hInfoWindow, outputBuff);
			//свёртка полосы прокрутки вниз
			int lineCount = SendMessageA(hInfoWindow, EM_GETLINECOUNT, 0, 0L);
			SendMessageA(hInfoWindow, EM_LINESCROLL, 0, MAKELPARAM(lineCount, 0));

		}
		//кнопку 3
		if (lparam == (LPARAM)hBtn3)
		{
			currVecCompElem = SendMessage(hList1, LB_GETCARETINDEX, 0, 0L);
			InitiateSystemShutdownA(vecComp[currVecCompElem].Info.Name, "", 0, TRUE, FALSE);
			SendMessageA(hList1, WM_CHILDACTIVATE, 0, 0L);

			strcat(outputBuff, "выключить ");
			strcat(outputBuff, vecComp[currVecCompElem].Info.Name);
			strcat(outputBuff, "\r\n");
			SetWindowTextA(hInfoWindow, outputBuff);
			//свёртка полосы прокрутки вниз
			int lineCount = SendMessageA(hInfoWindow, EM_GETLINECOUNT, 0, 0L);
			SendMessageA(hInfoWindow, EM_LINESCROLL, 0, MAKELPARAM(lineCount, 0));
		}
		//кнопку 4
		if (lparam == (LPARAM)hBtn4)
		{
			currVecCompElem = SendMessage(hList1, LB_GETCARETINDEX, 0, 0L);
			if (0 == PING(vecComp[currVecCompElem].Info.IP_address))
			{
				SendMessage(hwnd, MSG_PING, 1, 0L);
			}
			else
			{
				SendMessage(hwnd, MSG_PING, 0, 0L);
			}
		}
		//кнопку 5
		if (lparam == (LPARAM)hBtn5)
		{
			// Создаем модальную диалоговую панель
			DialogBox(hInst, "SELECT", hwnd, (DLGPROC)DlgProc);
		}
		break;
	case WM_CHARTOITEM:
		if (wparam == VK_DOWN)
		{
			currVecCompElem = SendMessage(hList1, LB_GETCARETINDEX, 0, 0L);
			SendMessage(hList1, LB_SETCARETINDEX, currVecCompElem++, FALSE);
		}
		if (wparam == VK_UP)
		{
			currVecCompElem = SendMessage(hList1, LB_GETCARETINDEX, 0, 0L);
			SendMessage(hList1, LB_SETCARETINDEX, currVecCompElem--, FALSE);
		}
		break;
	case WM_PAINT: // перерисовка окна
		hdc = BeginPaint(hwnd, &ps); // начало перерисовки

		EndPaint(hwnd, &ps); // конец перерисовки
		break;
	case WM_SIZE:
		switch (wparam)
		{
		case SIZE_MAXIMIZED:
			MoveWindow(hList1, 10, 10, LOWORD(lparam) - 215, HIWORD(lparam) - 165, TRUE);
			MoveWindow(hInfoWindow, 10, HIWORD(lparam) - 150, LOWORD(lparam) - 20, 140, TRUE);
			MoveWindow(hEdit, LOWORD(lparam) - 170, 10, 120, 30, TRUE);
			MoveWindow(hBtn1, LOWORD(lparam) - 170, 50, 120, 30, TRUE);
			MoveWindow(hBtn2, LOWORD(lparam) - 170, 90, 120, 30, TRUE);
			MoveWindow(hBtn3, LOWORD(lparam) - 170, 130, 120, 30, TRUE);
			MoveWindow(hBtn4, LOWORD(lparam) - 170, 170, 120, 30, TRUE);
			MoveWindow(hBtn5, LOWORD(lparam) - 170, 210, 120, 50, TRUE);
			break;
		case SIZE_RESTORED:
			MoveWindow(hList1, 10, 10, LOWORD(lparam) - 215, HIWORD(lparam) - 165, TRUE);
			MoveWindow(hInfoWindow, 10, HIWORD(lparam) - 150, LOWORD(lparam) - 20, 140, TRUE);
			MoveWindow(hEdit, LOWORD(lparam) - 170, 10, 120, 30, TRUE);
			MoveWindow(hBtn1, LOWORD(lparam) - 170, 50, 120, 30, TRUE);
			MoveWindow(hBtn2, LOWORD(lparam) - 170, 90, 120, 30, TRUE);
			MoveWindow(hBtn3, LOWORD(lparam) - 170, 130, 120, 30, TRUE);
			MoveWindow(hBtn4, LOWORD(lparam) - 170, 170, 120, 30, TRUE);
			MoveWindow(hBtn5, LOWORD(lparam) - 170, 210, 120, 50, TRUE);
			break;
		}
		break;

	case MSG_PING:
		if (wparam)
		{
			strcat(outputBuff, vecComp[currVecCompElem].Info.Name);
			strcat(outputBuff, "  работает");
			strcat(outputBuff, "\r\n");
			SetWindowTextA(hInfoWindow, outputBuff);
			//свёртка полосы прокрутки вниз
			int lineCount = SendMessageA(hInfoWindow, EM_GETLINECOUNT, 0, 0L);
			SendMessageA(hInfoWindow, EM_LINESCROLL, 0, MAKELPARAM(lineCount, 0));
		}
		else
		{
			strcat(outputBuff, vecComp[currVecCompElem].Info.Name);
			strcat(outputBuff, "  НЕ работает");
			strcat(outputBuff, "\r\n");
			SetWindowTextA(hInfoWindow, outputBuff);
			//свёртка полосы прокрутки вниз
			int lineCount = SendMessageA(hInfoWindow, EM_GETLINECOUNT, 0, 0L);
			SendMessageA(hInfoWindow, EM_LINESCROLL, 0, MAKELPARAM(lineCount, 0));
		}
		break;

	case WM_DESTROY: // закрытие окна
		PostQuitMessage(0);
		break;
		
	default: // обработка сообщения по умолчанию
		return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}


/*------------------------------------
 Функция DlgProc
------------------------------------*/

BOOL CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// Инициализация диалоговой панели
		case WM_INITDIALOG:
		{
			return TRUE;
		}

		case WM_COMMAND:
		{
			switch (wParam)
			{
				UINT hours, minutes;
				BOOL hErr, mErr;
				SYSTEMTIME currentTime;

				char bufHour[3], bufMin[3];

				// Сообщение от кнопки "OK"
				case IDOK:
				{
					// Получаем время из текстового редактора 
					hours = GetDlgItemInt(hdlg, IDC_HOUR, &hErr, FALSE);
					minutes = GetDlgItemInt(hdlg, IDC_MINUTE, &mErr, FALSE);

					GetDlgItemTextA(hdlg, IDC_HOUR, bufHour, 3);
					GetDlgItemTextA(hdlg, IDC_MINUTE, bufMin, 3);

					char Str[72] = "Все компьютеры будут выключены в ";

					if (hours > 23 || minutes > 59 || hours < 0 || minutes < 0)
					{
						MessageBox(hdlg, "Введите время в правильном формате", "Ошибка", MB_OK);
						return FALSE;
					}

					GetLocalTime(&currentTime);

					if (hours < currentTime.wHour)
					{
						MessageBox(hdlg, "Введённый вами час уже прошёл", "Ошибка", MB_OK);
						return FALSE;
					}

					if ((hours == currentTime.wHour) && (minutes <= currentTime.wMinute))
					{
						MessageBox(hdlg, "Введённая вами минута уже прошла", "Ошибка", MB_OK);
						return FALSE;
					}

					//кол-во минут до старта
					int minutCount = (hours * 60 + minutes) - (currentTime.wHour * 60 + currentTime.wMinute);
					//кол-во секунд до старта
					int secCount = minutCount * 60;

					//время выключения компьютеров
					int timeX = time(NULL) + secCount;

					strcat(Str, bufHour);
					strcat(Str, ":");
					strcat(Str, bufMin);
					strcat(Str, ". Приложение будет заблокировано");

					EndDialog(hdlg, 0);
					MessageBox(hdlg, Str, "", MB_OK);

					while (1)
					{
						if (time(NULL) > timeX)
							break;
					}

					std::vector<computer> vecComp;

					getObjectsFromFile(vecComp);
					vecComp.shrink_to_fit();
					sortVecComp(vecComp);
					int vecSize = vecComp.size();

					char bufV[3];
					_itoa(vecSize, bufV, 10);
					MessageBoxA(NULL, bufV, "size", MB_OK);

					for (int i = 0; i < vecSize; i++)
					{
						InitiateSystemShutdownA(vecComp[i].Info.Name, "", 0, TRUE, FALSE);
					}

					HANDLE hToken;              
					TOKEN_PRIVILEGES tkp;       // указатель на структуру токена

					/* Получаем текущий дескриптор процесса, 
					чтобы мы могли получить привилегии отключения*/
					if (!OpenProcessToken(GetCurrentProcess(),
						TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
						return FALSE;

					// Получаем LUID для привелегии отключения 
					LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
						&tkp.Privileges[0].Luid);
					tkp.PrivilegeCount = 1;  
					tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

					// Получаем привелегию отключения для этого процесса
					AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
						(PTOKEN_PRIVILEGES)NULL, 0);

					InitiateSystemShutdownA(NULL, "", 0, TRUE, FALSE);
					return TRUE;
				}

				// Отмена диалоговой панели.
				case IDCANCEL:
				{
					// Устанавливаем флаг завершения диалога
					EndDialog(hdlg, FALSE);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
