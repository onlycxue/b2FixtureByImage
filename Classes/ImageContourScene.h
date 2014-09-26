#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "CCImage.h"
#include "Box2D\Box2D.h"
#include "GLES-Render.h"
#include "b2FixtureByImage.h"

USING_NS_CC;

class ImageContourScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();  
	void box2dInit();
    void menuCloseCallback(cocos2d::Ref* pSender);
	void draw(Renderer *renderer,const kmMat4 &transform,bool transformUpdated);
	//Vec2 getStartingPixel(Texture2D* texture);
	//int getSquareValue(Vec2 pos);
	//std::vector<Vec2> marchingSquares(Texture2D* texture);
	//char getAlphaValue(int n);
	//Color4B getColorbyIndex(int index);
	//int getIndexbyPos(int x, int y);

	//b2Vec2* convertToB2Vec2(std::vector<Vec2> array);
	//Vec2* convertToVec2(std::vector<Vec2> array);
	//Vec2* convertToVec2(std::vector<b2Vec2> array);
	//void draw(Renderer *renderer,const kmMat4 &transform,bool transformUpdated);
	//void onDraw(const kmMat4 &transform, bool transformUpdated);
	//std::vector<b2Vec2> RDP(std::vector<b2Vec2> v,float epsilon);
	//float findPerpendicularDistance(b2Vec2 p,b2Vec2 p1,b2Vec2 p2);

    CREATE_FUNC(ImageContourScene);
	
private:
	  //unsigned char* _data;
	  //char _tolerance;
	  //Texture2D* _texture;
	  //std::vector<Vec2> _contourArray;
	  b2World* _world;
	  b2Body* _box2DBody;
	  b2FixtureByImage* _fixtureByImage;
	  //Sprite* _sprite;
	  //CustomCommand _customCommand;
	  //std::vector<b2Vec2> _RDPArray;
	  //std::vector<Vec2> _testArray;
	  //Vec2 testPoints[4];
	 // bool _able;

};

#endif // __HELLOWORLD_SCENE_H__
