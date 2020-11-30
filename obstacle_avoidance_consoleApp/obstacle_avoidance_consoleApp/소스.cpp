#include<iostream>
#include<ctime>
#include"mcd_engine.h"
using namespace std;

int nowY = 100, nowX = 400;//주인공 캐릭터 시작위치

void init() {

	initWindow("장애물 피하기 게임", 640, 480);//콘솔창 사이즈 지정
	addImage(0, "resources\\back.bmp", false);//배경이미지 등록
	addImage(1, "resources\\char.bmp", true);//주인공 캐릭터 이미지 등록
	addImage(2, "resources\\char2.bmp", true);//악당 캐릭터 이미지 등록
	finishAddImage();

	addObject(0, 0);//오브젝트에 배경이미지 등록
	addObject(1, 1, 4, 4);

	for (int i = 0; i < 10; i++) {
		addObject(2 + i, 2, 4, 4);
	}

	showObject(0, 0, 0, 0, 0);
}
//struct devilInform {
//	int direct;
//	int move;
//	int devilX;
//	int devilY;
//	unsigned int tick;
//};
//devilInform inform[10];
////direct=0->아래 1->왼쪽 2->오른쪽 3->위
//void movingDevilRandom(devilInform inform, int id, int speed) {
//	
//	for (int i = 0; i < inform.move; i++) {
//		if (isCrash(id, 1) == 1) {
//			OutputDebugString("!");
//		}
//		showObject(id, inform.devilX, inform.devilY, inform.tick % 4, inform.direct);
//		inform.devilY += speed;
//		if (inform.devilX < 0 || inform.devilY < 0 || inform.devilX >= 640 || inform.devilY >= 480)break;
//		Sleep(100);
//		inform.tick++;
//
//	}
//}

//악당이 랜덤으로 움직임
void moveDevil(int id) {
	srand(clock());

	int devilY = 300, devilX = 300;//악당 캐릭터 시작위치
	unsigned int tick = 0;
	int speed = 10;
	//direct=0->아래 1->왼쪽 2->오른쪽 3->위
	while (1) {
		int move = rand() % 50 + 20;
		int direct = rand() % 4;
		if (direct == 0) {//아래
			for (int i = 0; i < move; i++) {
				if (isCrash(id, 1) == 1) {
					OutputDebugString("!");
				}
				showObject(id, devilX, devilY, tick % 4, direct);
				devilY += speed;
				if (devilX < 0 || devilY < 0 || devilX >= 640 || devilY >= 480)break;
				Sleep(100);
				tick++;

			}
		}
		else if (direct == 1) {//왼쪽
			for (int i = 0; i < move; i++) {
				if (isCrash(id, 1) == 1) {
					OutputDebugString("!");
				}
				showObject(id, devilX, devilY, tick % 4, direct);
				devilX-= speed;
				if (devilX < 0 || devilY < 0 || devilX >= 640 || devilY >= 480)break;
				Sleep(100);
				tick++;

			}
		}
		else if (direct == 2) {//오른쪽
			for (int i = 0; i < move; i++) {
				if (isCrash(id, 1) == 1) {
					OutputDebugString("!");
				}
				showObject(id, devilX, devilY, tick % 4, direct);
				devilX += speed;
				if (devilX < 0 || devilY < 0 || devilX >= 640 || devilY >= 480)break;
				Sleep(100);
				tick++;

			}
		}
		else if (direct == 3) {//위
			for (int i = 0; i < move; i++) {
				if (isCrash(id, 1) == 1) {
					OutputDebugString("!");
				}
				showObject(id, devilX, devilY, tick % 4, direct);
				devilY -= speed;
				if (devilX < 0 || devilY < 0 || devilX >= 640 || devilY >= 480)break;
				Sleep(100);
				tick++;

			}
		}



	}
}
int main() {
	init();
	//미션1 캐릭터가 숨쉬는 듯한 움직임 주기
	//미션2 캐릭터 3마리 추가하기
	//미션3 캐릭터 움직이도록
	//미션4 캐릭터 애니메이션 효과

	unsigned int tick = 0;
	int flagIsMoving = 0;
	int direct = 0;
	int speed = 10;

	thread th[100];
	for (int i = 0; i < 10; i++) {
		th[i] = thread(moveDevil, 2 + i);//동시에 동작함
		
		Sleep(20);
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

		showObject(1, nowX, nowY, tick % 4, direct);

		tick++;
		Sleep(100);
		flagIsMoving = 0;
	}

	while (1);
	return 0;
}