#ifndef __GAME_LAYER2_H__
#define __GAME_LAYER2_H__

#include "cocos2d.h"
#include "Box2D\Box2D.h"
#include <GLES-Render.h>

#define PTM_RATIO 32

using namespace std;
using namespace cocos2d;

enum DIR {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
};

typedef struct _ATTR {
	int hp;
	float scale;
	int drop;
	DIR ani;
} ATTR;

class GameLayer2 : public cocos2d::LayerColor, public b2ContactListener
{
public:
	~GameLayer2();
	static GameLayer2* create(Color4B);

    virtual bool init(Color4B);
	virtual void update(float);

	int impactCnt;
	int useBall;
	float x;
	float ballSpeed;

	b2World* _world;
	b2ContactListener* b2listener;
	void initBox2DWorld(bool);
	void box2dUpdate(float);

	//graphic
	Sprite* createEffect(string fname, int column, int row, float fps, int roof);

	//for impact
	int num;
	void BeginContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, b2Manifold* mainFold);
	void PostSolve(b2Contact* contact, b2ContactImpulse* impulse);
	void EndContact(b2Contact* contact);
	void Impact(b2Body* sprA, b2Body* sprB);

	//for box2d joint
	bool isDrag;
	b2Body* gBody;
	b2Body* dragBody;
	b2MouseJoint* mouseJoint;
	b2Body* getBodyAtTab(Vec2 p);
	b2Body* addNewSprite(Vec2 pos, Size size, b2BodyType bodyType, Sprite* sprite, int type, float restitution);

	//for debug
	GLESDebugDraw* m_debugDraw;
	CustomCommand _customCmd;
	virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
	void onDraw(const Mat4& transform, uint32_t flags);



	//game controller
};

#endif // __GAME_LAYER2_H__
