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
	Image* image = new Image();
	image->initWithImageFile("hero1.png");
	_data = image->getData();
	CCLog("%s",_data);
	_tolerance = 0x00;

	_texture = new Texture2D();
	_texture->initWithImage(image);
	
	_sprite = Sprite::create("hero1.png");
	_sprite->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2));
	//addChild(_sprite,10);
	box2dInit();
	_contourArray = marchingSquares(_texture);
	
	b2Vec2* b2Vec2Array = convertToB2Vec2(_contourArray);
	
	//for(int i = 0 ; i < _contourArray.size() - _contourArray.size()%6;i = i+6)
	//{
	//	b2PolygonShape shape;
	//	shape.Set(b2Vec2Array+i,6);
	b2FixtureDef* fixtureDef = new b2FixtureDef();
	fixtureDef->density = 1.0f;
	fixtureDef->friction = 3.0f;
	fixtureDef->restitution = 0.6;
	//	_box2DBody->CreateFixture(&fixtureDef);
	//}
	b2Separator* separator = new b2Separator();
	std::vector<b2Vec2> *vec = new std::vector<b2Vec2>();
	
	for(int i = _contourArray.size()-1; i >=0; i--)
	{
		vec->push_back(b2Vec2(_contourArray[i].x,_contourArray[i].y));
	}
	std::vector<b2Vec2> vec_1;
	for(int i = 0 ; i < vec->size()/2; i++)
	{
		vec_1.push_back(vec->at(i));
	}
	vec_1 = RDP(vec_1,8.0);
	std::vector<b2Vec2> vec_2;
	for(int i = vec->size()/2; i < vec->size(); i++)
	{
		vec_2.push_back(vec->at(i));
	}
	vec_2 = RDP(vec_2,8.0);
	_RDPArray = vec_1;
	_RDPArray.insert(_RDPArray.end(),vec_2.begin(),vec_2.end());
	//for(int i = 0; i < _contourArray.size(); i++)
	//{
	//	vec->push_back(b2Vec2(_contourArray[i].x,_contourArray[i].y));
	//}

	//清除接近的点
/*	CCLog("tempVector size is %d",tempVector.size());	
	for(int i = 0; i < tempVector.size(); i++)
	{
		CCLog("tempVector:(%f,%f)",tempVector[i].x,tempVector[i].y);
	}*/
	//_RDPArray = RDP(*vec,5.2);
	CCLog("Vec2 size is %d",_RDPArray.size());
	
	for(int i = 0; i < _RDPArray.size(); i++)
	{
		CCLog("_RDPVector:(%f,%f)",_RDPArray.at(i).x,_RDPArray.at(i).y);
	}
	if(separator->Validate(_RDPArray) == 0) {
		CCLog("Yey ! Those vertices are good to go!");
		separator->Separate(_box2DBody,fixtureDef,&_RDPArray,PTM_RATIO);
	}
	else
	{
		CCLog("Oh , I guess you messed something up %d",separator->Validate(_RDPArray));
		
		
	}
	_able = true;
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
	box2dBodyDef.type = b2_staticBody;
	box2dBodyDef.position.Set(screenSize.width/2/PTM_RATIO,screenSize.height/2/PTM_RATIO);
	box2dBodyDef.position.Set(0,0);
	box2dBodyDef.userData = _sprite;

	_box2DBody = _world->CreateBody(&box2dBodyDef);

}

Scene* ImageContourScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ImageContourScene::create();
	scene->addChild(layer,10);
	return scene;

}
Vec2 ImageContourScene::getStartingPixel(Texture2D* texture)
{
	for(int i = 0; i < texture->getPixelsHigh(); i++)
	{
		for(int j = 0 ; j < texture->getPixelsWide(); j++)
		{
			int index = i*texture->getPixelsWide() + j;
			if(getColorbyIndex(index).a == _tolerance)
			{
				continue;
			}
			return Vec2(j,i);			
		}
	}
	return Vec2(-1,-1);

}
int ImageContourScene::getSquareValue(Vec2 pos)
{
	int squareNum = 0;
	//int index = pos.x * _texture->getPixelsHigh + pos.y;
	if(getColorbyIndex(getIndexbyPos(pos.x - 1 , pos.y -1)).a > _tolerance)
	{
		squareNum += 1;
	}
	if(getColorbyIndex(getIndexbyPos(pos.x , pos.y -1)).a > _tolerance)
	{
		squareNum += 2;
	}
	if(getColorbyIndex(getIndexbyPos(pos.x - 1 , pos.y)).a > _tolerance)
	{
		squareNum += 4;
	}
	if(getColorbyIndex(getIndexbyPos(pos.x, pos.y)).a > _tolerance)
	{
		squareNum += 8;
	}
	return squareNum;
}
b2Vec2* ImageContourScene::convertToB2Vec2(std::vector<Vec2> array)
{
	//b2Vec2 contourArray[array.size];
	b2Vec2 *contourArray = (b2Vec2*)malloc(array.size()*sizeof(b2Vec2));
	for(int i = 0 ; i < array.size(); i++)
	{
		contourArray[i] = b2Vec2(array.at(i).x,array.at(i).y);
	}
	return contourArray;
}
Vec2* ImageContourScene::convertToVec2(std::vector<Vec2> array)
{
	Vec2* contourArray = (Vec2*)malloc(array.size()*sizeof(Vec2));
	for(int i = 0; i < array.size(); i++)
	{
		contourArray[i] = Vec2(array.at(i).x,array.at(i).y);
	}
	return contourArray;
}
Vec2* ImageContourScene::convertToVec2(std::vector<b2Vec2> array)
{
	Vec2* contourArray = (Vec2*)malloc(array.size()*sizeof(Vec2));
	for(int i = 0; i < array.size(); i++)
	{
		contourArray[i] = Vec2(array.at(i).x,array.at(i).y);
	}
	return contourArray;	
}
int ImageContourScene::getIndexbyPos(int x, int y)
{
	return y*_texture->getPixelsWide() + x;
}
std::vector<Vec2> ImageContourScene::marchingSquares(Texture2D* texture)
{
	Vec2 startPos = getStartingPixel(_texture);
	if(startPos != Vec2(-1,-1))
	{
		int pX = (int)startPos.x;
		int pY = (int)startPos.y;
		CCLog("the startPos is (%d,%d)",pX,pY);
		int stepX =0;
		int stepY =0;

		int prevX ;
		int prevY ;

		bool closedLoop = false;
		while(!closedLoop) {
			int squareValue = getSquareValue(Vec2(pX,pY));
			CCLog("squareVale number is %d",squareValue);
			switch (squareValue)
			{
				case 1 :
				case 5 :
				case 13 :
					stepX = 0;
					stepY = -1;
					break;
				case 8 :
				case 10:
				case 11:
					stepX = 0;
					stepY = 1;
					break;
				case 4:
				case 12:
				case 14:
					stepX = -1;
					stepY = 0;
					break;
				case 2:
				case 3:
				case 7:
					stepX=1;
					stepY=0;
					break;
				case 6:
					if(prevX == 0 && prevY ==-1)
					{
						stepX = -1;
						stepY = 0;
					}
					else
					{
						stepX = 1;
						stepY = 0;
					}
					break;
				case 9:
					if(prevX == 1 && prevY == 0)
					{
						stepX = 0;
						stepY = -1;
					}
					else
					{
						stepX = 0;
						stepY = 1;
					}
					break;
				
			//default:
			//	break;

			}
			pX += stepX;
			pY += stepY;
			//Vec2 pos = _sprite->convertToWorldSpace(Vec2(pX,_sprite->getContentSize().height-pY));
			
			_contourArray.push_back(Vec2(pX,pY));
			prevX = stepX;
			prevY = stepY;
			if(pX == startPos.x && pY ==  startPos.y)
			{
				closedLoop = true;
			}
		}
	}
	return _contourArray;
}
char ImageContourScene::getAlphaValue(int n)
{
	return n >> 24 & 0xff; 
}
Color4B ImageContourScene::getColorbyIndex(int index)
{
	unsigned char r = _data[index*4 + 0];
	unsigned char g = _data[index*4 + 1];
	unsigned char b = _data[index*4 + 2];
	unsigned char a = _data[index*4 + 3];
	CCLog("the index is %d and the alapha is %x",index,a);
	return Color4B(r,g,b,a);
}

std::vector<b2Vec2> ImageContourScene::RDP(std::vector<b2Vec2> v,float epsilon)
{
	b2Vec2 firstPoint = v[0];
	b2Vec2 lastPoint = v[v.size() - 1];
	if(v.size() < 3)
	{
		return v;
	}
	float index = -1.0f;
	float dist = 0.0f;
	for(int i = 1; i < v.size()-1 ; i++)
	{
		float cDist = findPerpendicularDistance(v[i],firstPoint,lastPoint);
		if(cDist > dist)
		{
			dist = cDist;
			index = i;
		}
	}
	if(dist > epsilon)
	{
		std::vector<b2Vec2> l1(v.begin(),v.begin()+index);
		std::vector<b2Vec2> l2(v.begin()+index,v.end());
		std::vector<b2Vec2> r1 = RDP(l1,epsilon);
		std::vector<b2Vec2> r2 = RDP(l2,epsilon);
		std::vector<b2Vec2> rs(r1.begin(),r1.end());
		rs.insert(rs.end(),r2.begin(),r2.end());
		return rs;
	}
	else
	{
		std::vector<b2Vec2> n;
		n.push_back(firstPoint);
		n.push_back(lastPoint);
		return n;
	}
	return std::vector<b2Vec2>();
}
float ImageContourScene::findPerpendicularDistance(b2Vec2 p,b2Vec2 p1,b2Vec2 p2)
{
	float result;
	float slope;
	float intercept;
	if(p1.x == p2.x)
	{
		result = fabs(p.x - p1.x);
	}
	else
	{
		slope = (p2.y - p1.y)/(p2.x - p1.x);
		intercept = p1.y - (slope*p1.x);
		result = fabs(slope * p.x - p.y + intercept) / sqrt(pow(slope,2)+1);
	}
	return result;
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
void ImageContourScene::draw(Renderer *renderer,const kmMat4 &transform,bool transformUpdated)
{
	//_customCommand.init(_globalZOrder);
    //_customCommand.func = CC_CALLBACK_0(ImageContourScene::onDraw, this, transform, transformUpdated);
   // renderer->addCommand(&_customCommand);
	//glLineWidth(1);
    //Point filledVertices[] = { Point(0,120), Point(50,120), Point(50,170), Point(25,200), Point(0,170) };
   // DrawPrimitives::drawSolidPoly(filledVertices, 5, Color4F(0.5f, 0.5f, 1, 1 ) );

	//glLineWidth(2);
   // DrawPrimitives::setDrawColor4B(255, 0, 255, 255);
    //DrawPrimitives::drawSolidCircle( Point(140,0), 40, CC_DEGREES_TO_RADIANS(90), 50, 1.0f, 1.0f);

	//Point points[] = { Point(60,60), Point(70,70), Point(60,70), Point(70,60) };
   // DrawPrimitives::setPointSize(4);
    //DrawPrimitives::setDrawColor4B(0,255,255,255);
    //DrawPrimitives::drawPoints( points, 4);
	//Vec2 testPoints[] = {Vec2(60,60),Vec2(170,170),Vec2(160,170),Vec2(170,160)}; 
	//ccPointSize(40);
	//ccDrawColor4B(255,0,0,0);
	//CCLog("the contourArray size is %d",_contourArray.size());
	//ccDrawPoints(testPoints,4);
	_world->DrawDebugData();
	DrawPrimitives::setPointSize(1);
    DrawPrimitives::setDrawColor4B(0,255,255,255);
	DrawPrimitives::drawPoints( convertToVec2(_contourArray),_contourArray.size());
	
	DrawPrimitives::setPointSize(1);
    DrawPrimitives::setDrawColor4B(255,0,0,255);
	DrawPrimitives::drawPoints( convertToVec2(_RDPArray),_RDPArray.size());
	//DrawPrimitives::drawPoly(convertToVec2(_contourArray),_contourArray.size(),true);
	CHECK_GL_ERROR_DEBUG();
}
void ImageContourScene::onDraw(const kmMat4 &transform, bool transformUpdated)
{
	//kmGLPushMatrix();
    //kmGLLoadMatrix(&transform);
	ccPointSize(1);
	ccDrawColor4B(255,0,0,0);
	CCLog("the contourArray size is %d",_contourArray.size());
	ccDrawPoints(convertToVec2(_contourArray),_contourArray.size());
	//DrawPrimitives::drawPoly(convertToVec2(_contourArray),_contourArray.size(),true);
	CHECK_GL_ERROR_DEBUG();
	//kmGLPopMatrix();

    
}