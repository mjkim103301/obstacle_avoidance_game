
//mincoding game engine2 - (rev.191116)

#pragma once
#pragma comment(lib, "msimg32.lib")

#include <Windows.h>
#include <string>
#include <vector>
#include <thread>
#include <unordered_map>
using namespace std;

unordered_map<int, int> imgNum;
unordered_map<int, int> objNum;
string msg;

struct ObjectInfo
{
	int imageID;
	int objectID;

	int spriteXCnt;
	int spriteYCnt;

	int spriteX;
	int spriteY;

	bool isEnable;
	int x;
	int y;

	int unit_width;
	int unit_height;
};

struct ImageInfo
{
	int imageID;
	HDC hDC;
	HBITMAP hBtm;
	BITMAP btm;
	bool isTrans;
	COLORREF tranColor; //투명컬러
};

vector<ImageInfo> img;
vector<ObjectInfo> obj;

HWND hWnd;
HDC  hWndDC;
HDC  hMemDC;
HBITMAP hMemBtm;

int winWidth;
int winHeight;

thread printThread;
int exitFlagThread;

void finalizeWindow()
{
	exitFlagThread = 1;
}

//창 제목과 가로, 세로 사이즈 입력
void initWindow(const char* title, int width, int height)
{
	winWidth = width;
	winHeight = height;

	hWnd = GetConsoleWindow();
	hWndDC = GetDC(hWnd);

	//창 드래그 막기 (빠른 편집 비활성화)
	DWORD prevMode;
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prevMode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prevMode & ~ENABLE_QUICK_EDIT_MODE);

	//창 스타일 변경
	LONG prevStyle = GetWindowLong(hWnd, GWL_STYLE);
	SetWindowLongA(hWnd, GWL_STYLE, WS_CAPTION | WS_DLGFRAME);

	//창 사이즈 X * Y으로 변경
	SetWindowPos(hWnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_SHOWWINDOW);

	//창 시작이름 변경
	SetWindowTextA(hWnd, title);

	//입력 커서 숨기기
	CONSOLE_CURSOR_INFO curInfo = { 1 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

	//memDC 초기화
	hMemDC = CreateCompatibleDC(hWndDC);
	hMemBtm = CreateCompatibleBitmap(hWndDC, width, height);
	SelectObject(hMemDC, hMemBtm);

	Sleep(10);
}

//글 내용과 좌표와 글자크기, RGB Color 입력 (0 ~ 255)
void printText(const char* str, int x, int y, int fontSize, int red, int green, int blue)
{
	SetTextColor(hMemDC, RGB(red, green, blue)); //컬러
	SetBkMode(hMemDC, TRANSPARENT); //배경색

	HFONT font = CreateFontA(fontSize, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 3, 2, PROOF_QUALITY, 0, "맑은 고딕");

	SelectObject(hMemDC, font);
	TextOutA(hMemDC, x, y, str, strlen(str));

	DeleteObject(font);
}

void setText(const char* str, bool isNew)
{
	if (isNew == true) msg = "";
	else {
		msg += str;
	}
}

COLORREF getPixelObject(HBITMAP hBtmDC, int x, int y)
{
	//이미지 사이즈 얻기
	BITMAP bm;
	GetObjectA(hBtmDC, sizeof(BITMAP), &bm);
	int width = bm.bmWidth;
	int height = bm.bmHeight;

	//bitmapheader 만들기
	BITMAPINFOHEADER bminfoheader;
	::ZeroMemory(&bminfoheader, sizeof(BITMAPINFOHEADER));
	bminfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bminfoheader.biWidth = width;
	bminfoheader.biHeight = -height;
	bminfoheader.biPlanes = 1;
	bminfoheader.biBitCount = 32;
	bminfoheader.biCompression = BI_RGB;
	bminfoheader.biSizeImage = width * 4 * height;
	bminfoheader.biClrUsed = 0;
	bminfoheader.biClrImportant = 0;

	//임시 버퍼를 만들고, GetDIBits으로 버퍼에 색상값 채우기
	unsigned char* pPixels = new unsigned char[(width * 4 * height)];
	GetDIBits(CreateCompatibleDC(0), hBtmDC, 0, height, pPixels, (BITMAPINFO*)&bminfoheader, DIB_RGB_COLORS); // load pixel info 

	//좌표계산
	unsigned char r = pPixels[(width * y + x) * 4 + 2];
	unsigned char g = pPixels[(width * y + x) * 4 + 1];
	unsigned char b = pPixels[(width * y + x) * 4 + 0];

	//임시 버퍼 제거
	delete[] pPixels;

	return RGB(r, g, b);
}

//고유 이미지ID와 이미지 파일을 등록한다.
void addImage(int imageID, const char* fileName, bool isTransparent)
{
	//신규 ID 추가
	if (imgNum.count(imageID) == 1) {
		MessageBoxA(hWnd, "이미 추가된 Image ID입니다. 고유의 ImageID를 사용해주세요", "ERROR", NULL);
		exit(1);
	}
	imgNum[imageID] = img.size();

	ImageInfo x = { 0 };

	x.imageID = imageID;
	x.hDC = CreateCompatibleDC(hWndDC);

	//BITMAP Object 생성
	x.hBtm = (HBITMAP)LoadImageA(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (x.hBtm == NULL) {
		string tmp = fileName;
		tmp += " 파일을 찾을 수 없습니다.";
		MessageBoxA(hWnd, tmp.c_str(), "ERROR", NULL);
		exit(1);
	}

	//비트맵 정보 얻기
	GetObjectA(x.hBtm, sizeof(BITMAP), &x.btm);

	//DC에 Object Select
	SelectObject(x.hDC, x.hBtm);

	if (isTransparent == true) {
		x.isTrans = true;
		x.tranColor = getPixelObject(x.hBtm, 0, 0); //투명 색상 구하기 (0,0) pixel을 투명컬러로 간주한다.
	}
	else {
		x.isTrans = false;
	}

	img.push_back(x);
}

//Object를 추가한다. Strite 처리되는 이미지라면 분할되는 개수도 입력한다.
void addObject(int objectID, int imageID, int splitXCnt = 1, int splitYCnt = 1)
{
	if (objNum.count(objectID) == 1) {
		MessageBoxA(hWnd, "addObject() :: 이미 추가된 Object입니다. 중복된 ObjectID를 부여했는지 확인하세요", "ERROR", NULL);
		exit(1);
	}

	if (imgNum.count(imageID) == 0) {
		MessageBoxA(hWnd, "addObject() :: 없는 imageID입니다. 먼저 ImageID를 생성 후 사용해주세요", "ERROR", NULL);
		exit(1);
	}

	objNum[objectID] = obj.size();

	ObjectInfo x = { 0 };

	x.objectID = objectID;
	x.imageID = imageID;
	x.spriteXCnt = splitXCnt;
	x.spriteYCnt = splitYCnt;

	int imgIndex = imgNum[imageID];

	x.unit_width = img[imgIndex].btm.bmWidth / splitXCnt;
	x.unit_height = img[imgIndex].btm.bmHeight / splitYCnt;

	obj.push_back(x);
}

//선택된 Object를 움직임을 결정하고, 화면에 보이게 설정한다.
void showObject(int objectID, int x, int y, int spriteX, int spriteY) {
	if (objNum.count(objectID) == 0) {
		MessageBoxA(hWnd, "없는 Object ID입니다", "ERROR", NULL);
		exit(1);
	}

	int index = objNum[objectID];
	obj[index].isEnable = true;
	obj[index].x = x;
	obj[index].y = y;
	obj[index].spriteX = spriteX;
	obj[index].spriteY = spriteY;

	if (spriteX < 0 || spriteY < 0 || spriteX >= obj[index].spriteXCnt || spriteY >= obj[index].spriteYCnt) {
		string tmp = "showObject함수 에러 :: (spriteX : " + to_string(spriteX) + ", spriteY : " + to_string(spriteY) + ")값은 사용이 불가능한 값입니다.";
		MessageBoxA(hWnd, tmp.c_str(), "ERROR", NULL);
		exit(1);
	}
}

void PrintAll()
{
	unsigned int clock = 0;

	while (1)
	{
		int i, t;
		int index;
		int width, height;
		int targetX, targetY;

		//출력하고자 하는 모든 obj들을 memDC에 그려두기
		for (auto t : objNum) {
			i = t.second;
			if (obj[i].isEnable == true) {

				index = imgNum[obj[i].imageID];

				width = img[index].btm.bmWidth / obj[i].spriteXCnt;
				height = img[index].btm.bmHeight / obj[i].spriteYCnt;
				targetX = width * obj[i].spriteX;
				targetY = height * obj[i].spriteY;

				if (img[index].isTrans == false) {
					BitBlt(hMemDC, obj[i].x, obj[i].y, width, height, img[index].hDC, targetX, targetY, SRCCOPY);
				}
				else {
					TransparentBlt(hMemDC, obj[i].x, obj[i].y, width, height, img[index].hDC, targetX, targetY, width, height, img[index].tranColor);
				}
			}
		}

		//memDC 이미지를 화면에 출력
		hWndDC = GetDC(hWnd);
		printText(msg.c_str(), 10, 10, 50, 255, 255, 255);
		BitBlt(hWndDC, 0, 0, winWidth, winHeight, hMemDC, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hWndDC);

		Sleep(1);
		clock++;

		if (exitFlagThread == 1) break;
	}

}

//무한Loop를 돌며, 출력하고자 하는 모든 Object를 memDC에 모은 후, memDC를 출력한다.
void finishAddImage()
{
	if (img.size() == 0) {
		MessageBoxA(hWnd, "등록할 이미지가 없습니다. addImage를 먼저 해 주세요", "ERROR", NULL);
		exit(1);
	}

	printThread = thread(PrintAll);
}

//ObjectID에 대한 가로 x 세로길이를 리턴한다.
int getImageSize(int objectID, int* width, int* height)
{
	if (objNum.count(objectID) == 0) {
		MessageBoxA(hWnd, "getImageSize() Error :: 없는 Object ID입니다", "ERROR", NULL);
		exit(1);
	}

	int index = objNum[objectID];
	*width = obj[index].unit_width;
	*height = obj[index].unit_height;
}

int isCrash(int objID1, int objID2)
{
	int percent = 70; //70% 사각 영역끼리 겹치면 충돌로 판단

	if (objNum.count(objID1) == 0) {
		MessageBoxA(hWnd, "isCrash() :: 찾을 수 없는 ObjectID - 1", "ERROR", NULL);
		exit(1);
	}

	if (objNum.count(objID2) == 0) {
		MessageBoxA(hWnd, "isCrash() :: 찾을 수 없는 ObjectID - 2", "ERROR", NULL);
		exit(1);
	}

	int oid1 = objNum[objID1];
	int oid2 = objNum[objID2];

	int ax = obj[oid1].x;
	int ay = obj[oid1].y;
	int bx = obj[oid2].x;
	int by = obj[oid2].y;

	int aw = obj[oid1].unit_width;
	int ah = obj[oid1].unit_height;
	int bw = obj[oid2].unit_width;
	int bh = obj[oid2].unit_height;

	int apw = aw * percent / 100;
	int bpw = bw * percent / 100;
	int aph = ah * percent / 100;
	int bph = bh * percent / 100;

	int left = (100 - percent) / 2 + 1;
	int apx = ax + (aw * left / 100);
	int apy = ay + (ah * left / 100);
	int bpx = bx + (bw * left / 100);
	int bpy = by + (bh * left / 100);

	RECT ret;
	RECT a = { apx, apy, apx + apw, apy + aph };
	RECT b = { bpx, bpy, bpx + bpw, bpy + bph };

	IntersectRect(&ret, &a, &b);

	if (ret.left == 0 && ret.right == 0 && ret.top == 0 && ret.bottom == 0) return 0;
	return 1;
}