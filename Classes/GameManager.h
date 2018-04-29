#pragma once
#include "cocos2d.h"
using namespace std;
USING_NS_CC;

#define SPRINTF StringUtils::format

struct MAPINFO
{
	int index;
	int cleared;
	vector<int> nextIndex;
	int preDegree;
	int preIndex;
	int creditReward;
	int diaRewardRate;
};

extern bool soundOnOff;
extern bool fxOnOff;
extern int _audioID;
extern bool callbackend[5];

extern Size visibleSize;
extern Vec2 origin;


extern MAPINFO mapinfo;//ķ���� �������� ����
extern int stageNum;//�������� �ѹ�
extern int stageLevel;//�������� ���̵�
extern int stageKind;//�������� ���� (���-ķ����, ���潺, ����)
extern int credit;//ũ����Ʈ ����
extern int dia;//���� ����
extern int campaign_normal[100];
extern int campaign_hard[100];
extern int campaign_hell[100];
extern int campaign_dia[100];

extern int haveitem[100];//�������� ������ ����
extern int equipWeapon[3];//����ϴ� ������ ����

extern int spinnum;//���� ����
extern double spinviewtime;//���� ���� ��� Ÿ��

extern bool isDebug;


void initGame();
void stopMusic(int);
void playMusic(string, float, bool);
void playEffect(string, float);
void playEffectABS(string fname, float vol);

string getResourceAsString(string valuename);
void makeString();
int getValue(int raw);
Vec2 getDest(Vec2 pos, int angle, float speed);
int getAngle(Vec2 pos1, Vec2 pos2);