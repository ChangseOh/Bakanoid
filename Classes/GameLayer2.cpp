#include "GameLayer2.h"
#include "SimpleAudioEngine.h"
#include "ui\CocosGUI.h"
#include "GameManager.h"

using namespace std;
USING_NS_CC;
using namespace ui;

GameLayer2::~GameLayer2()
{
	delete _world;
	_world = nullptr;

	delete b2listener;
	b2listener = nullptr;
}
GameLayer2* GameLayer2::create(Color4B color)
{
	GameLayer2* layer = new GameLayer2();

	if (layer->init(color))
	{
		return layer;
	}

	CC_SAFE_DELETE(layer);
	return NULL;
}

// on "init" you need to initialize your instance
bool GameLayer2::init(Color4B color)
{
    //////////////////////////////
    // 1. super init first
    if ( !LayerColor::initWithColor(color) )
    {
        return false;
    }

	//Director::getInstance()->getScheduler()->setTimeScale(0.25f);

	auto bg = Sprite::create("games/bg_4.jpg");// Screenshot_2018 - 04 - 02 - 08 - 51 - 53.png");
	bg->setScale(visibleSize.height / bg->getContentSize().height);
	bg->setPosition(Vec2(visibleSize * 0.5f) + origin);
	bg->setOpacity(100);
	addChild(bg);

	fxOnOff = true;
	impactCnt = 0;
	useBall = 0;
	ballSpeed = 24.0f;

	initBox2DWorld(false);
	_world->SetContactListener((b2ContactListener*)this);
	num = 0;

	//gBody = addNewSprite(Vec2::ZERO, Size(10, 10), b2_staticBody, "CloseNormal.png", 0);
	auto sprite = Sprite::create("games/ball.png");
	sprite->setScale(1.5f);
	sprite->getTexture()->setAliasTexParameters();
	sprite->setName("BALL");
	auto ballBody = addNewSprite(Vec2(visibleSize.width * 0.5f, 450) + origin, Size(30, 30), b2_dynamicBody, sprite, 0, 1.0f);
	auto npos = getDest(Vec2::ZERO, 45, ballSpeed);
	ballBody->SetLinearVelocity(b2Vec2(0, 0));// npos.x, npos.y));
	ballBody->SetGravityScale(0);//무중력

	string fnames[7] = {
		"games/brick64b.png",
		"games/brick64g.png",
		"games/brick64gold.png",
		"games/brick64k.png",
		"games/brick64r.png",
		"games/brick64s.png",
		"games/brick64y.png"
	};

	for (int i = 0; i < 50; i++)
	{
		int hp = 4 - (i / 20);
		if (hp < 1)
			hp = 1;
		if (hp > 4)
			hp = 4;
		auto sprite = Sprite::create(fnames[i / 10]);
		sprite->getTexture()->setAliasTexParameters();
		sprite->setScale(1.6f);
		sprite->setName("BRICK");
		sprite->setTag(0);
		//sprite->setVisible(false);
		ATTR* attr = new ATTR;
		attr->hp = hp;
		sprite->setUserData(attr);
		Vec2 pos = Vec2((i % 10) * 104 + 72, visibleSize.height - ((i / 10) * 80 + 300));
		auto brickBody = addNewSprite(pos, Size(102, 38), b2_kinematicBody, sprite, 1, 0);
		
	}

	Vector<SpriteFrame*> aniframes;
	SpriteFrameCache::getInstance()->addSpriteFrame(SpriteFrame::create("games/bat_penguind_0.png", Rect(0, 0, 92, 39)), "BAR0");
	SpriteFrameCache::getInstance()->addSpriteFrame(SpriteFrame::create("games/bat_penguind_1.png", Rect(0, 0, 92, 39)), "BAR1");
	SpriteFrameCache::getInstance()->addSpriteFrame(SpriteFrame::create("games/bat_penguind_2.png", Rect(0, 0, 92, 39)), "BAR2");
	SpriteFrameCache::getInstance()->addSpriteFrame(SpriteFrame::create("games/bat_penguind_3.png", Rect(0, 0, 92, 39)), "BAR3");

	aniframes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("BAR0"));
	aniframes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("BAR0"));
	aniframes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("BAR0"));
	aniframes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("BAR1"));
	aniframes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("BAR2"));
	aniframes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("BAR2"));
	aniframes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("BAR1"));

	auto barsprite = Sprite::create("games/bat_penguind_0.png");
	barsprite->getTexture()->setAliasTexParameters();
	barsprite->setName("BAR");
	barsprite->setScale(2.4f);
	barsprite->setAnchorPoint(Vec2(0.5, 0.35));
	auto barBody = addNewSprite(Vec2(visibleSize.width * 0.5f, 400), Size(200, 50), b2_kinematicBody, barsprite, 1, 1.0f);
	barBody->SetGravityScale(0);//무중력

	Animation* ani = Animation::createWithSpriteFrames(aniframes, 0.3f, 1);
	barsprite->runAction(RepeatForever::create(Animate::create(ani)));

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch* touch, Event* event) {

		x = touch->getLocation().x;
		return true;
	};
	listener->onTouchMoved = [=](Touch* touch, Event* event) {

		float _x = touch->getLocation().x;
		auto pos = barBody->GetPosition();
		pos.x = pos.x + (_x - x) / PTM_RATIO;
		//pos.y = pos.y + touch->getDelta().y;
		barBody->SetTransform(pos, barBody->GetAngle());
		x = _x;
	};
	listener->onTouchEnded = [=](Touch* touch, Event* event) {

		if (useBall == 0)
		{
			useBall = 1;
			ballBody->SetLinearVelocity(b2Vec2(npos.x, npos.y));
			playEffectABS("sound/ready.mp3", 0.9f);
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

b2Body* GameLayer2::addNewSprite(Vec2 pos, Size size, b2BodyType bodyType, Sprite* sprite, int type, float restitution)
{
	b2BodyDef bodyDef;
	bodyDef.type = bodyType;
	bodyDef.position.Set(pos.x / PTM_RATIO, pos.y / PTM_RATIO);

	if (sprite != nullptr)
		addChild(sprite);
	bodyDef.userData = sprite;

	b2Body* body = _world->CreateBody(&bodyDef);
	b2FixtureDef fixtureDef;
	b2CircleShape circle;
	b2PolygonShape dynamicBox;

	if (type == 0) {
		//circle shape
		circle.m_radius = size.width / 2 / PTM_RATIO;
		fixtureDef.shape = &circle;
	}
	if (type == 1) {
		//poly shape
		dynamicBox.SetAsBox(size.width / 2 / PTM_RATIO, size.height / 2 / PTM_RATIO);
		fixtureDef.shape = &dynamicBox;
	}

	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = restitution;// 1.0f;

	body->CreateFixture(&fixtureDef);

	return body;
}

void GameLayer2::update(float dt)
{
	box2dUpdate(dt);

}
void GameLayer2::box2dUpdate(float dt)
{

	int velocityIterations = 8;
	int positionIterations = 3;

	_world->Step(dt, velocityIterations, positionIterations);

	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != nullptr)
		{
			Sprite* spriteData = (Sprite*)b->GetUserData();
			spriteData->setPosition(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO);
			if (spriteData->getName().compare("BALL") == 0)
				spriteData->setRotation(spriteData->getRotation() + 3);
			if (spriteData->getName().compare("BRICK") == 0)
				spriteData->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
			if (spriteData->getTag() == -1 && spriteData->getName().compare("BRICK") == 0)
			{
				b->SetType(b2_dynamicBody);
				spriteData->setTag(0);
			}
			ATTR* attr = (ATTR*)spriteData->getUserData();
			if (attr != nullptr && attr->hp <= 0 || spriteData->getPositionY() < -100)
			{
				CC_SAFE_DELETE(attr);

				spriteData->removeFromParent();
				b2Body* nextBody = b->GetNext();
				_world->DestroyBody(b);
				b = nextBody;
			}
		}
	}
}

void GameLayer2::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
	Layer::draw(renderer, transform, flags);
	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(GameLayer2::onDraw, this, transform, flags);
	renderer->addCommand(&_customCmd);
}
void GameLayer2::onDraw(const Mat4& transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION);
	_world->DrawDebugData();
	CHECK_GL_ERROR_DEBUG();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
void GameLayer2::initBox2DWorld(bool debugmode)
{
	Size vsize = visibleSize;

	auto gravity = b2Vec2(0.0f, -20.0f);
	_world = new b2World(gravity);
	_world->SetAllowSleeping(true);
	_world->SetContinuousPhysics(true);

	if (debugmode)
	{
		m_debugDraw = new GLESDebugDraw(PTM_RATIO);
		_world->SetDebugDraw(m_debugDraw);

		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;
		m_debugDraw->SetFlags(flags);
	}

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);

	b2Body* groundBody = _world->CreateBody(&groundBodyDef);

	b2EdgeShape groundEdge;
	b2FixtureDef boxShapeDef;
	boxShapeDef.shape = &groundEdge;

	groundEdge.Set(b2Vec2(0, 0), b2Vec2(vsize.width / PTM_RATIO, 0));
	groundBody->CreateFixture(&boxShapeDef);

	groundEdge.Set(b2Vec2(0, 0), b2Vec2(0, vsize.height / PTM_RATIO));
	groundBody->CreateFixture(&boxShapeDef);

	groundEdge.Set(b2Vec2(0, vsize.height / PTM_RATIO), b2Vec2(vsize.width / PTM_RATIO, vsize.height / PTM_RATIO));
	groundBody->CreateFixture(&boxShapeDef);

	groundEdge.Set(b2Vec2(vsize.width / PTM_RATIO, vsize.height / PTM_RATIO), b2Vec2(vsize.width / PTM_RATIO, 0));
	groundBody->CreateFixture(&boxShapeDef);

	scheduleUpdate();
}
void GameLayer2::BeginContact(b2Contact* contact)
{
	log("BeginContact");
	num = 1;
}
void GameLayer2::PreSolve(b2Contact* contact, b2Manifold* mainFold)
{
	log("PreSolve");
	num = 2;
	//auto fixA = contact->GetFixtureA();
	//auto fixB = contact->GetFixtureB();
	//auto bodyA = fixA->GetBody();
	//auto bodyB = fixB->GetBody();

	//Sprite* sprA = (Sprite*)bodyA->GetUserData();
	//Sprite* sprB = (Sprite*)bodyB->GetUserData();

	//if (sprA == nullptr || sprB == nullptr)
	//	return;

	//Sprite* brick = nullptr;
	//b2Body* brickBody;

	//if ((sprA->getName().compare("BRICK") == 0 && sprB->getName().compare("BAR") == 0)
	//	|| (sprA->getName().compare("BAR") == 0 && sprB->getName().compare("BRICK") == 0))
	//{
	//	contact->SetEnabled(false);
	//	return;
	//}

	//if ((sprA->getName().compare("BALL") == 0 && sprB->getName().compare("BAR") == 0)
	//	|| (sprA->getName().compare("BAR") == 0 && sprB->getName().compare("BALL") == 0))
	//{
	//	float poss = 0;
	//	playEffectABS("sound/reflect.mp3", 0.6f);
	//	if (sprB->getName().compare("BAR") == 0)
	//	{
	//		float xzero = sprB->getPosition().x - sprB->getContentSize().width * 0.5f * sprB->getScaleX();
	//		poss = (sprA->getPositionX() - xzero) / (sprB->getContentSize().width * sprB->getScaleX()) - 0.5f;
	//		auto npos = getDest(Vec2::ZERO, (int)(poss * 140), ballSpeed);
	//		//bodyA->SetLinearVelocity(b2Vec2(0, 0));
	//		bodyA->SetLinearVelocity(b2Vec2(npos.x, npos.y));
	//		//log("%d", (int)(360 + poss * 80) % 360);
	//	}
	//	else
	//	{
	//		float xzero = sprA->getPosition().x - sprA->getContentSize().width * 0.5f * sprA->getScaleX();
	//		poss = (sprB->getPositionX() - xzero) / (sprA->getContentSize().width * sprA->getScaleX()) - 0.5f;
	//		auto npos = getDest(Vec2::ZERO, (int)(poss * 140), ballSpeed);
	//		//bodyB->SetLinearVelocity(b2Vec2(0, 0));
	//		bodyB->SetLinearVelocity(b2Vec2(npos.x, npos.y));
	//		//log("%d", (int)(360 + poss * 80) % 360);
	//	}
	//	return;
	//}

	////충돌하면 해당 바디를 다이나믹으로 교체
	//if (sprA->getName().compare("BRICK") == 0 && sprB->getName().compare("BALL") == 0)
	//{
	//	brick = sprA;
	//	brickBody = bodyA;
	//	//brick->setTag(-1);
	//}
	//if (sprB->getName().compare("BRICK") == 0 && sprA->getName().compare("BALL") == 0)
	//{
	//	brick = sprB;
	//	brickBody = bodyB;
	//	//brick->setTag(-1);
	//}
	//if (brick != nullptr)
	//{
	//	ATTR* attr = (ATTR*)brick->getUserData();
	//	if (attr != nullptr)
	//	{
	//		attr->hp--;
	//		if (attr->hp < 2)
	//			brick->setTag(-1);
	//	}
	//}
	//playEffectABS("sound/reflect2.mp3", 0.6f);



	//b2Fixture* fixtureA = contact->GetFixtureA();
	//b2Fixture* fixtureB = contact->GetFixtureB();

	//b2Body* bodyA = fixtureA->GetBody();
	//b2Body* bodyB = fixtureB->GetBody();

	//if (bodyA->GetUserData() == nullptr || bodyB->GetUserData() == nullptr) {
	//	return;
	//}

	//Sprite* spr1 = (Sprite *)bodyA->GetUserData();
	//Sprite* spr2 = (Sprite *)bodyB->GetUserData();

	//float pos1 = spr1->getPosition().y + (spr1->getContentSize().height / 2);
	////float pos2 = spr2->getPosition().y - (spr2->getContentSize().height / 2);
	//float pos2 = spr2->getPosition().y;

	//if (pos2 < pos1)
	//{
	//	contact->SetEnabled(false);
	//}
	//[출처][3.15.1] #21 - One Way Platform 간단 구현(Cocos2D - X 사용자 모임) | 작성자 내폰젤무거워
}
void GameLayer2::PostSolve(b2Contact* contact, b2ContactImpulse* impulse)
{
	log("PostSolve");
}
void GameLayer2::EndContact(b2Contact* contact)
{
	log("EndContact");
}

Sprite* GameLayer2::createEffect(string fname, int column, int row, float fps, int roof)
{
	//fps = fps * column * row;

	auto effect = Sprite::create(fname);
	Size effectSize = effect->getContentSize();
	float width = effectSize.width / column;
	float height = effectSize.height / row;
	Vector<SpriteFrame*> animFrame;
	int num = 0;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < column; x++)
		{
			SpriteFrameCache::getInstance()->addSpriteFrame(SpriteFrame::create(fname, Rect(width * x, height * y, width, height)), StringUtils::format("ANIM%d", num));
			animFrame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("ANIM%d", num)));
			num++;
		}
	}
	effect->setSpriteFrame("ANIM0");
	if (roof == -1)
		effect->runAction(RepeatForever::create(
			Animate::create(Animation::createWithSpriteFrames(animFrame, fps, 1))
		));
	else
		effect->runAction(Sequence::create(
			Animate::create(Animation::createWithSpriteFrames(animFrame, fps, roof)),
			RemoveSelf::create(),
			NULL
		));

	return effect;
}

b2Body* GameLayer2::getBodyAtTab(Vec2 p)
{
	b2Fixture* fix;

	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != nullptr)
		{
			//if (b->GetType() == b2_staticBody)
			//	continue;
			fix = b->GetFixtureList();
			if (fix->TestPoint(b2Vec2(p.x / PTM_RATIO, p.y / PTM_RATIO)))
			{
				return b;
			}
		}
	}
	return nullptr;
}
void GameLayer2::Impact(b2Body* src, b2Body* target)
{

	Sprite* srcSpr = (Sprite*)src->GetUserData();
	Sprite* targetSpr = (Sprite*)target->GetUserData();

}
