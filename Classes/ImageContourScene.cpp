#include "ImageContourScene.h"

USING_NS_CC;
#define PTM_RATIO 32


bool ImageContourScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	box2dInit();
	_fixtureByImage = new b2FixtureByImage(_box2DBody,"hero0.png",4.0);
	return true;
}
void ImageContourScene::box2dInit()
{
	b2Vec2 gravity;
	gravity.Set(0.0f,-10.0f);

	bool doSleep = true;
	
	_world = new b2World(gravity);
	_world->SetAllowSleeping(doSleep);
	_world->SetContinuousPhysics(true);
	
	GLESDebugDraw* m_debugDraw = new GLESDebugDraw(PTM_RATIO);
	_world->SetDebugDraw(m_debugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	//flags += b2Draw::e_pairBit;
	//flags += b2Draw::e_centerOfMassBit;

	m_debugDraw->SetFlags(flags);

	Size screenSize = Director::getInstance()->getWinSize();
	b2BodyDef box2dBodyDef;
	box2dBodyDef.type = b2_dynamicBody;
	box2dBodyDef.position.Set(screenSize.width/2/PTM_RATIO,screenSize.height/2/PTM_RATIO);
	//box2dBodyDef.position.Set(0,0);
	//box2dBodyDef.userData = _sprite;

	_box2DBody = _world->CreateBody(&box2dBodyDef);

}

Scene* ImageContourScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ImageContourScene::create();
	scene->addChild(layer,10);
	return scene;

}
void ImageContourScene::draw(Renderer *renderer,const kmMat4 &transform,bool transformUpdated)
{
	_world->DrawDebugData();
	//画图片的轮廓
	DrawPrimitives::setPointSize(1);
    DrawPrimitives::setDrawColor4B(0,255,255,255);
	DrawPrimitives::drawPoints(_fixtureByImage->getImageContourPoints(),_fixtureByImage->getImageContourVector().size());

	//画RDP之后的点
	DrawPrimitives::setPointSize(1);
	DrawPrimitives::setDrawColor4B(255,0,0,255);
	DrawPrimitives::drawPoints(_fixtureByImage->getRDPPoints(),_fixtureByImage->getRDPVector().size());
}

void ImageContourScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
