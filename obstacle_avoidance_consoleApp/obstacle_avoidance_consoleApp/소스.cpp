#include<iostream>
#include<ctime>
#include"mcd_engine.h"
using namespace std;

int nowY = 100, nowX = 400;//���ΰ� ĳ���� ������ġ

void init() {

	initWindow("��ֹ� ���ϱ� ����", 640, 480);//�ܼ�â ������ ����
	addImage(0, "resources\\back.bmp", false);//����̹��� ���
	addImage(1, "resources\\char.bmp", true);//���ΰ� ĳ���� �̹��� ���
	addImage(2, "resources\\char2.bmp", true);//�Ǵ� ĳ���� �̹��� ���
	finishAddImage();

	addObject(0, 0);//������Ʈ�� ����̹��� ���
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
////direct=0->�Ʒ� 1->���� 2->������ 3->��
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

//�Ǵ��� �������� ������
void moveDevil(int id) {
	srand(clock());

	int devilY = 300, devilX = 300;//�Ǵ� ĳ���� ������ġ
	unsigned int tick = 0;
	int speed = 10;
	//direct=0->�Ʒ� 1->���� 2->������ 3->��
	while (1) {
		int move = rand() % 50 + 20;
		int direct = rand() % 4;
		if (direct == 0) {//�Ʒ�
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
		else if (direct == 1) {//����
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
		else if (direct == 2) {//������
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
		else if (direct == 3) {//��
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
	//�̼�1 ĳ���Ͱ� ������ ���� ������ �ֱ�
	//�̼�2 ĳ���� 3���� �߰��ϱ�
	//�̼�3 ĳ���� �����̵���
	//�̼�4 ĳ���� �ִϸ��̼� ȿ��

	unsigned int tick = 0;
	int flagIsMoving = 0;
	int direct = 0;
	int speed = 10;

	thread th[100];
	for (int i = 0; i < 10; i++) {
		th[i] = thread(moveDevil, 2 + i);//���ÿ� ������
		
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