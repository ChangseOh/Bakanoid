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


extern MAPINFO mapinfo;//캠페인 스테이지 정보
extern int stageNum;//스테이지 넘버
extern int stageLevel;//스테이지 난이도
extern int stageKind;//스테이지 종류 (모드-캠페인, 디펜스, 보스)
extern int credit;//크레디트 정보
extern int dia;//보석 정보
extern int campaign_normal[100];
extern int campaign_hard[100];
extern int campaign_hell[100];
extern int campaign_dia[100];

extern int haveitem[100];//소지중인 아이템 정보
extern int equipWeapon[3];//장비하는 아이템 갯수

extern int spinnum;//스핀 갯수
extern double spinviewtime;//스핀 충전 대기 타임

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