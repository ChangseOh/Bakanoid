#include "GameManager.h"
#include "audio/include/AudioEngine.h"

using namespace std;
USING_NS_CC;
using namespace cocos2d::experimental;

char codeKey[] = "df3c-ccc1-3a4f-00a8-42ec";


bool soundOnOff;
bool fxOnOff;
int _audioID;
bool callbackend[5];

Size visibleSize;
Vec2 origin;


MAPINFO mapinfo;//ķ���� �������� ����
int stageNum;//�������� �ѹ�
int stageLevel;//�������� ���̵�
int stageKind;//�������� ���� (���-ķ����, ���潺, ����)
int credit;//ũ����Ʈ ����
int dia;//���� ����
int campaign_normal[100];
int campaign_hard[100];
int campaign_hell[100];
int campaign_dia[100];

int haveitem[100];//�������� ������ ����
int equipWeapon[3];//����ϴ� ������ ����

int spinnum;//���� ����
double spinviewtime;//���� ���� ��� Ÿ��

bool isDebug;


void initGame()
{

}
void stopMusic(int id)
{
	AudioEngine::stop(id);
}
void playMusic(string fname, float vol, bool roof)
{
	if (!soundOnOff)
		return;

	if (_audioID == AudioEngine::INVALID_AUDIO_ID)
	{
		_audioID = AudioEngine::play2d(fname, roof, vol);
	}
	else
	{
		stopMusic(_audioID);
		_audioID = AudioEngine::play2d(fname, roof, vol);
	}

	if (!soundOnOff)
		AudioEngine::pauseAll();
}
void playEffect(string fname, float vol)
{
	if (!fxOnOff)
		return;

	for (int i = 0; i < 5; i++)
	{
		if (!callbackend[i])
		{
			int id = AudioEngine::play2d(fname, false, vol);
			if (id != AudioEngine::INVALID_AUDIO_ID)
			{
				callbackend[i] = true;
				AudioEngine::setFinishCallback(id, [=](int aa, string ac) {
					callbackend[i] = false;
				});
			}
		}
	}
}
void playEffectABS(string fname, float vol)
{
	if (!fxOnOff)
		return;

	AudioEngine::play2d(fname, false, vol);
}

string getResourceAsString(string valuename)
{
	string filename = "string_kr.xml";
	switch (Application::sharedApplication()->getCurrentLanguage())
	{
	case LanguageType::KOREAN:
		filename = "string_kr.xml";
		break;
	case LanguageType::ENGLISH:
		break;
	case LanguageType::JAPANESE:
		break;
	case LanguageType::CHINESE:
		break;
	default:
		filename = "string_kr.xml";
		break;
	}
	long size;
	auto buf = FileUtils::sharedFileUtils()->getValueMapFromFile(filename);
	if (buf[valuename].isNull())
		return "null string";

	return buf[valuename].asString();
}
void makeString()
{
	ValueMap datas;
	datas["GAMETITLE"] = "������";
	datas["BOSS1NAME"] = "����ġ";

	FileUtils::sharedFileUtils()->writeValueMapToFile(datas, "string_kr.xml");
}
int getValue(int raw)
{
	char temp[4];
	temp[0] = (char)(raw >> 24);
	temp[1] = (char)(raw >> 16);
	temp[2] = (char)(raw >> 8);
	temp[3] = (char)(raw);

	for (int i = 0; i<4; i++)
		temp[i] ^= codeKey[i % sizeof(codeKey)];

	return (
		((temp[0] & 0xff) << 24) +
		((temp[1] & 0xff) << 16) +
		((temp[2] & 0xff) << 8) +
		(temp[3] & 0xff)
		);
}
Vec2 getDest(Vec2 pos, int angle, float speed)
{
	float x = speed * sin(angle * 3.14159f / 180.0f);
	float y = speed * cos(angle * 3.14159f / 180.0f);
	log("%.1f %.1f", x, y);
	return (pos + Vec2(x, y));
}
int getAngle(Vec2 pos1, Vec2 pos2)
{
	float vx = pos1.x - pos2.x;
	float vy = pos1.y - pos2.y;
	double rad = atan2(vx, vy);
	return (int)((rad * 180) / 3.1415f);
}
