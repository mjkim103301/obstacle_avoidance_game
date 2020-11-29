#include<iostream>
#include<ctime>
#include"mcd_engine.h"
using namespace std;

int nowY = 100, nowX = 400;
void init() {

	initWindow("후후후", 640, 480);//콘솔창 사이즈 지정
	addImage(0, "resources\\back.bmp", false);//배경이미지 등록
	addImage(1, "resources\\char.bmp", true);//캐릭터 이미지 등록
	addImage(2, "resources\\char2.bmp", true);//캐릭터 이미지 등록
	finishAddImage();

	addObject(0, 0);//오브젝트에 배경이미지 등록
	addObject(1, 1, 4, 4);

	for (int i = 0; i < 10; i++) {
		addObject(2 + i, 2, 4, 4);
	}

	showObject(0, 0, 0, 0, 0);
}

void k_f_c(int id) {//악당이 앞으로 갔다 뒤로갔다 반복
	srand(clock());
	unsigned int tick = 0;
	//int direct = 0;//0이거나 3이거나
	int speed = 10;
	int devilY = rand() % 500, devilX = rand() % 500;//랜덤으로 악당의 위치를 정함
	while (1) {
		int move = rand() % 50 + 20;
		int direct = rand() % 4;


		//if (tick < move) {//밑으로
		//	direct = 0;
		//	devilY += speed;
		//}
		//else if(tick<move) {//위로
		//	direct = 3;
		//	devilY -= speed;
		//}
		//else {
		//	tick = 0;
		//	
		//}

		if (direct == 0) {//아래
			for (int i = 0; i < move; i++) {
				devilY += speed;
				if (devilY < 30)break;

			}
		}
		else if (direct == 1) {//왼쪽
			for (int i = 0; i < move; i++) {
				devilY += speed;
				if (devilY < 30)break;
			}
		}
		else if (direct == 2) {//오른쪽

		}
		else if (direct == 3) {//위

		}
		showObject(id, devilX, devilY, tick % 4, direct);
		tick++;

		Sleep(100);
		if (isCrash(id, 4) == 1) {
			OutputDebugString("!");
		}
	}
}
int main() {
	init();
	//미션1 캐릭터가 숨쉬는 듯한 움직임 주기
	//미션2 캐릭터 3마리 추가하기
	//미션3 캐릭터 움직이도록
	//미션4 캐릭터 애니메이션 효과
	//while (1) {
	//	for (int y = 0; y < 4; y++) {
	//		for (int x = 0; x < 4; x++) {
	//			showObject(1, 100, 100, x, y);
	//			showObject(2, 200, 100, 3, 0);
	//			showObject(3, 300, 100, 3, 0);
	//			Sleep(500);//1000 = 1초
	//		}
	//	}
	//}
	unsigned int tick = 0;
	int flagIsMoving = 0;
	int direct = 0;
	int speed = 10;

	thread th[100];
	for (int i = 0; i < 10; i++) {
		th[i] = thread(k_f_c, 2 + i);//동시에 동작함
		Sleep(30);
	}


	while (1) {

		if (GetAsyncKeyState(VK_UP) & 0x8001) {
			direct = 3;
			nowY -= speed;
			flagIsMoving = 1;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8001) {
			direct = 0;
			nowY += speed;
			flagIsMoving = 1;
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8001) {
			direct = 1;
			nowX -= speed;
			flagIsMoving = 1;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8001) {
			direct = 2;
			nowX += speed;
			flagIsMoving = 1;
		}
		if (flagIsMoving == 0) {
			tick = 0;
		}



		//showObject(1, 100, 100, 3, 0);
		showObject(2, 200, 100, 3, 0);
		showObject(3, 300, 100, 3, 0);
		showObject(4, nowX, nowY, tick % 4, direct);

		tick++;
		Sleep(100);
		flagIsMoving = 0;
	}



	while (1);
	return 0;
}