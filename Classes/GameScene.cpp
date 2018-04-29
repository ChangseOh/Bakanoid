#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "ui\CocosGUI.h"
#include "GameManager.h"
//#include "GameLayer.h"
#include "GameLayer2.h"
//#include "PlatformLayer.h"

using namespace std;
USING_NS_CC;
using namespace ui;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in GameSceneScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	auto layer = GameLayer2::create(Color4B::WHITE);
	//auto layer = GameLayer::create();
	addChild(layer);

    return true;
}
