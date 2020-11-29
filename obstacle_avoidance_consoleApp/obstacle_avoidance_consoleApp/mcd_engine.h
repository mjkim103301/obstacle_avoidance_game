
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
	COLORREF tranColor; //�����÷�
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

//â ����� ����, ���� ������ �Է�
void initWindow(const char* title, int width, int height)
{
	winWidth = width;
	winHeight = height;

	hWnd = GetConsoleWindow();
	hWndDC = GetDC(hWnd);

	//â �巡�� ���� (���� ���� ��Ȱ��ȭ)
	DWORD prevMode;
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prevMode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prevMode & ~ENABLE_QUICK_EDIT_MODE);

	//â ��Ÿ�� ����
	LONG prevStyle = GetWindowLong(hWnd, GWL_STYLE);
	SetWindowLongA(hWnd, GWL_STYLE, WS_CAPTION | WS_DLGFRAME);

	//â ������ X * Y���� ����
	SetWindowPos(hWnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_SHOWWINDOW);

	//â �����̸� ����
	SetWindowTextA(hWnd, title);

	//�Է� Ŀ�� �����
	CONSOLE_CURSOR_INFO curInfo = { 1 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

	//memDC �ʱ�ȭ
	hMemDC = CreateCompatibleDC(hWndDC);
	hMemBtm = CreateCompatibleBitmap(hWndDC, width, height);
	SelectObject(hMemDC, hMemBtm);

	Sleep(10);
}

//�� ����� ��ǥ�� ����ũ��, RGB Color �Է� (0 ~ 255)
void printText(const char* str, int x, int y, int fontSize, int red, int green, int blue)
{
	SetTextColor(hMemDC, RGB(red, green, blue)); //�÷�
	SetBkMode(hMemDC, TRANSPARENT); //����

	HFONT font = CreateFontA(fontSize, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 3, 2, PROOF_QUALITY, 0, "���� ���");

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
	//�̹��� ������ ���
	BITMAP bm;
	GetObjectA(hBtmDC, sizeof(BITMAP), &bm);
	int width = bm.bmWidth;
	int height = bm.bmHeight;

	//bitmapheader �����
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

	//�ӽ� ���۸� �����, GetDIBits���� ���ۿ� ���� ä���
	unsigned char* pPixels = new unsigned char[(width * 4 * height)];
	GetDIBits(CreateCompatibleDC(0), hBtmDC, 0, height, pPixels, (BITMAPINFO*)&bminfoheader, DIB_RGB_COLORS); // load pixel info 

	//��ǥ���
	unsigned char r = pPixels[(width * y + x) * 4 + 2];
	unsigned char g = pPixels[(width * y + x) * 4 + 1];
	unsigned char b = pPixels[(width * y + x) * 4 + 0];

	//�ӽ� ���� ����
	delete[] pPixels;

	return RGB(r, g, b);
}

//���� �̹���ID�� �̹��� ������ ����Ѵ�.
void addImage(int imageID, const char* fileName, bool isTransparent)
{
	//�ű� ID �߰�
	if (imgNum.count(imageID) == 1) {
		MessageBoxA(hWnd, "�̹� �߰��� Image ID�Դϴ�. ������ ImageID�� ������ּ���", "ERROR", NULL);
		exit(1);
	}
	imgNum[imageID] = img.size();

	ImageInfo x = { 0 };

	x.imageID = imageID;
	x.hDC = CreateCompatibleDC(hWndDC);

	//BITMAP Object ����
	x.hBtm = (HBITMAP)LoadImageA(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (x.hBtm == NULL) {
		string tmp = fileName;
		tmp += " ������ ã�� �� �����ϴ�.";
		MessageBoxA(hWnd, tmp.c_str(), "ERROR", NULL);
		exit(1);
	}

	//��Ʈ�� ���� ���
	GetObjectA(x.hBtm, sizeof(BITMAP), &x.btm);

	//DC�� Object Select
	SelectObject(x.hDC, x.hBtm);

	if (isTransparent == true) {
		x.isTrans = true;
		x.tranColor = getPixelObject(x.hBtm, 0, 0); //���� ���� ���ϱ� (0,0) pixel�� �����÷��� �����Ѵ�.
	}
	else {
		x.isTrans = false;
	}

	img.push_back(x);
}

//Object�� �߰��Ѵ�. Strite ó���Ǵ� �̹������ ���ҵǴ� ������ �Է��Ѵ�.
void addObject(int objectID, int imageID, int splitXCnt = 1, int splitYCnt = 1)
{
	if (objNum.count(objectID) == 1) {
		MessageBoxA(hWnd, "addObject() :: �̹� �߰��� Object�Դϴ�. �ߺ��� ObjectID�� �ο��ߴ��� Ȯ���ϼ���", "ERROR", NULL);
		exit(1);
	}

	if (imgNum.count(imageID) == 0) {
		MessageBoxA(hWnd, "addObject() :: ���� imageID�Դϴ�. ���� ImageID�� ���� �� ������ּ���", "ERROR", NULL);
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

//���õ� Object�� �������� �����ϰ�, ȭ�鿡 ���̰� �����Ѵ�.
void showObject(int objectID, int x, int y, int spriteX, int spriteY) {
	if (objNum.count(objectID) == 0) {
		MessageBoxA(hWnd, "���� Object ID�Դϴ�", "ERROR", NULL);
		exit(1);
	}

	int index = objNum[objectID];
	obj[index].isEnable = true;
	obj[index].x = x;
	obj[index].y = y;
	obj[index].spriteX = spriteX;
	obj[index].spriteY = spriteY;

	if (spriteX < 0 || spriteY < 0 || spriteX >= obj[index].spriteXCnt || spriteY >= obj[index].spriteYCnt) {
		string tmp = "showObject�Լ� ���� :: (spriteX : " + to_string(spriteX) + ", spriteY : " + to_string(spriteY) + ")���� ����� �Ұ����� ���Դϴ�.";
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

		//����ϰ��� �ϴ� ��� obj���� memDC�� �׷��α�
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

		//memDC �̹����� ȭ�鿡 ���
		hWndDC = GetDC(hWnd);
		printText(msg.c_str(), 10, 10, 50, 255, 255, 255);
		BitBlt(hWndDC, 0, 0, winWidth, winHeight, hMemDC, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hWndDC);

		Sleep(1);
		clock++;

		if (exitFlagThread == 1) break;
	}

}

//����Loop�� ����, ����ϰ��� �ϴ� ��� Object�� memDC�� ���� ��, memDC�� ����Ѵ�.
void finishAddImage()
{
	if (img.size() == 0) {
		MessageBoxA(hWnd, "����� �̹����� �����ϴ�. addImage�� ���� �� �ּ���", "ERROR", NULL);
		exit(1);
	}

	printThread = thread(PrintAll);
}

//ObjectID�� ���� ���� x ���α��̸� �����Ѵ�.
int getImageSize(int objectID, int* width, int* height)
{
	if (objNum.count(objectID) == 0) {
		MessageBoxA(hWnd, "getImageSize() Error :: ���� Object ID�Դϴ�", "ERROR", NULL);
		exit(1);
	}

	int index = objNum[objectID];
	*width = obj[index].unit_width;
	*height = obj[index].unit_height;
}

int isCrash(int objID1, int objID2)
{
	int percent = 70; //70% �簢 �������� ��ġ�� �浹�� �Ǵ�

	if (objNum.count(objID1) == 0) {
		MessageBoxA(hWnd, "isCrash() :: ã�� �� ���� ObjectID - 1", "ERROR", NULL);
		exit(1);
	}

	if (objNum.count(objID2) == 0) {
		MessageBoxA(hWnd, "isCrash() :: ã�� �� ���� ObjectID - 2", "ERROR", NULL);
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