#ifndef _B2FIXTUREBYIMAGE_H
#define _B2FIXTUREBYIMAGE_H
#include "Box2D\Box2D.h"
#include <string>
#include <vector>
#include "cocos2d.h"
#include "b2Separator.h"

USING_NS_CC;
#define PTM_RATIO 32

class b2FixtureByImage
{
public:
	b2FixtureByImage(b2Body* body,std::string imageName,float factor = 0.5);

	~b2FixtureByImage(){};
	Vec2* getImageContourPoints();
	std::vector<Vec2> getImageContourVector();
	std::vector<b2Vec2> getRDPVector();
	Vec2* getRDPPoints();
	
private:		
	Vec2 getStartingPixel(Texture2D* texture);
	int getSquareValue(Vec2 pos);
	std::vector<Vec2> marchingSquares(Texture2D* texture);
	char getAlphaValue(int n);
	Color4B getColorbyIndex(int index);
	int getIndexbyPos(int x, int y);
	//RDP 算法用来减少轮廓点的像素
	std::vector<Vec2> RDP(std::vector<Vec2> v,float epsilon);
	float findPerpendicularDistance(Vec2 p,Vec2 p1,Vec2 p2);

	std::vector<Vec2> sliceVectorForRDP(std::vector<Vec2> v);
	void convertTob2Vec2(std::vector<b2Vec2>* target,std::vector<Vec2> source);
	//向量的转化
	Vec2* convertToVec2(std::vector<Vec2> array);
	Vec2* convertToVec2(std::vector<b2Vec2> array);

private:
	std::string _imageName;
	b2Body* _b2body;
	float _RDPfactor;
	unsigned char* _data;
	char _tolerance;
	Texture2D* _texture;
	Sprite* _sprite;
	b2FixtureDef *_fixture;
	b2Separator *_separator;
	std::vector<Vec2> _contourArray;
	std::vector<b2Vec2>* _RDPArray;
	Vec2* _drawContourPoints;
	Vec2* _drawRDPPoints;
};

#endif