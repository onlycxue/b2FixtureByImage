#include "b2FixtureByImage.h"
#include "CCImage.h"
	
b2FixtureByImage::b2FixtureByImage(b2Body* body,std::string imageName,float factor):
	_b2body(body),_imageName(imageName)
{
	_tolerance = 0x00;

	Image* image = new Image();
	image->initWithImageFile("hero0.png");
	_data = image->getData();

	_texture = new Texture2D();
	_texture->initWithImage(image);
	//获取image轮廓点
	_contourArray = marchingSquares(_texture);

	_fixture = new b2FixtureDef();
	_fixture->density = 1.0f;
	_fixture->friction = 3.0f;
	_fixture->restitution = 0.6;
	 
	std::vector<Vec2> rdpVec;
	rdpVec = sliceVectorForRDP(_contourArray);

	_RDPArray = new std::vector<b2Vec2>();
		
	convertTob2Vec2(_RDPArray,rdpVec);
	_separator = new b2Separator();

	//验证数组是否通过
	if(_separator->Validate(*_RDPArray) == 0)
	{
		CCLog("Yey ! Those vertices are good to go!");
		_separator->Separate(_b2body,_fixture,_RDPArray,PTM_RATIO);
	}
	else
	{
		CCLog("Oh , I guess you messed something up %d",_separator->Validate(*_RDPArray));
	}
	//for draw();
	_drawContourPoints = convertToVec2(_contourArray);
	_drawRDPPoints = convertToVec2(*_RDPArray);

}
//获取开始点的像素坐标
Vec2 b2FixtureByImage::getStartingPixel(Texture2D* texture)
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
//根据坐标获取索引值
int b2FixtureByImage::getIndexbyPos(int x, int y)
{
	return y*_texture->getPixelsWide() + x;
}
//获取匹配的方块的值
int b2FixtureByImage::getSquareValue(Vec2 pos)
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

//方块匹配获取图片的轮廓点
std::vector<Vec2> b2FixtureByImage::marchingSquares(Texture2D* texture)
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
			Vec2 pos = _sprite->convertToWorldSpace(Vec2(pX,_sprite->getContentSize().height-pY));
			
			_contourArray.push_back(pos);
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

//获取某个像素点的alpha
char b2FixtureByImage::getAlphaValue(int n)
{
	return n >> 24 & 0xff; 
}
//获取index像素的颜色
Color4B b2FixtureByImage::getColorbyIndex(int index)
{
	unsigned char r = _data[index*4 + 0];
	unsigned char g = _data[index*4 + 1];
	unsigned char b = _data[index*4 + 2];
	unsigned char a = _data[index*4 + 3];
	return Color4B(r,g,b,a);
}
//用来减少点的数量
std::vector<Vec2> b2FixtureByImage::RDP(std::vector<Vec2> v,float epsilon)
{
	Vec2 firstPoint = v[0];
	Vec2 lastPoint = v[v.size() - 1];
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
		std::vector<Vec2> l1(v.begin(),v.begin()+index);
		std::vector<Vec2> l2(v.begin()+index,v.end());
		std::vector<Vec2> r1 = RDP(l1,epsilon);
		std::vector<Vec2> r2 = RDP(l2,epsilon);
		std::vector<Vec2> rs(r1.begin(),r1.end());
		rs.insert(rs.end(),r2.begin(),r2.end());
		return rs;
	}
	else
	{
		std::vector<Vec2> n;
		n.push_back(firstPoint);
		n.push_back(lastPoint);
		return n;
	}
	return std::vector<Vec2>();
}
float b2FixtureByImage::findPerpendicularDistance(Vec2 p,Vec2 p1,Vec2 p2)
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
std::vector<Vec2> b2FixtureByImage::sliceVectorForRDP(std::vector<Vec2> v)
{
	std::vector<Vec2> vec_1;
	for(int i = 0 ; i < v.size()/2; i++)
	{
		vec_1.push_back(v.at(i));
	}
	vec_1 = RDP(vec_1,8.0);
	std::vector<Vec2> vec_2;
	for(int i = v.size()/2; i < v.size(); i++)
	{
		vec_2.push_back(v.at(i));
	}
	vec_2 = RDP(vec_2,8.0);
	std::vector<Vec2> vec;
	vec = vec_1;
	vec.insert(vec.end(),vec_2.begin(),vec_2.end());
	return vec;
}

void b2FixtureByImage::convertTob2Vec2(std::vector<b2Vec2>* target,std::vector<Vec2> source)
{
	for(int i = 0; i < source.size(); i++)
	{
		target->push_back(b2Vec2(source.at(i).x,source.at(i).y));
	}
}
void b2FixtureByImage::draw(Renderer *renderer,const kmMat4 &transform,bool transformUpdated)
{
	//画图片的轮廓
	DrawPrimitives::setPointSize(1);
    DrawPrimitives::setDrawColor4B(0,255,255,255);
	DrawPrimitives::drawPoints(_drawContourPoints,_contourArray.size());

	//画RDP之后的点
	DrawPrimitives::setPointSize(1);
	DrawPrimitives::setDrawColor4B(255,0,0,255);
	DrawPrimitives::drawPoints(_drawRDPPoints,_RDPArray->size());
}

Vec2* b2FixtureByImage::convertToVec2(std::vector<Vec2> array)
{
	Vec2* contourArray = (Vec2*)malloc(array.size()*sizeof(Vec2));
	for(int i = 0; i < array.size(); i++)
	{
		contourArray[i] = Vec2(array.at(i).x,array.at(i).y);
	}
	return contourArray;
}
Vec2* b2FixtureByImage::convertToVec2(std::vector<b2Vec2> array)
{
	Vec2* contourArray = (Vec2*)malloc(array.size()*sizeof(Vec2));
	for(int i = 0; i < array.size(); i++)
	{
		contourArray[i] = Vec2(array.at(i).x,array.at(i).y);
	}
	return contourArray;
}