#include "b2Separator.h"
#include <algorithm>
#include <float.h>
void b2Separator::Separator(b2Body* body,b2FixtureDef fixtureDef,std::vector<b2Vec2> verticesVec,int scale)
{
	std::vector<b2Vec2> scaledVertices;
	for(int i = 0; i < verticesVec.size(); i++)
	{
		scaledVertices.push_back(b2Vec2(verticesVec.at(i).x,verticesVec.at(i).y));
	}
	std::vector<std::vector<b2Vec2>> shapeList = calcShapes(scaledVertices);
	
	for(int i = 0; i < shapeList.size(); i++)
	{
		std::vector<b2Vec2> normalizedVertices;
		std::vector<b2Vec2> shape = shapeList.at(i);
		for(int j = 0; j < shape.size();j++)
		{
			b2Vec2 scaled = shape.at(i);
			normalizedVertices.push_back(b2Vec2(scaled.x/scale,scaled.y/scale));
		}
		std::vector<b2Vec2> normal = removeZeroLengthEdges(normalizedVertices);
		b2PolygonShape Shape;
		Shape.Set(convertTob2Vec2Ptr(normal),normal.size());
		fixtureDef.shape = &Shape;
		body->CreateFixture(&fixtureDef);
	}
}
b2Vec2* b2Separator::convertTob2Vec2Ptr(std::vector<b2Vec2> array)
{
	//b2Vec2 contourArray[array.size];
	b2Vec2 *contourArray = (b2Vec2*)malloc(array.size()*sizeof(b2Vec2));
	for(int i = 0 ; i < array.size(); i++)
	{
		contourArray[i] = b2Vec2(array.at(i).x,array.at(i).y);
	}
	return contourArray;
}
std::vector<b2Vec2> b2Separator::removeZeroLengthEdges(std::vector<b2Vec2> normalizedVertices)
{
	if(normalizedVertices.size() > 1)
	{
		b2Vec2 first = normalizedVertices.front();
		if(first == normalizedVertices[normalizedVertices.size()-1])
		{
			normalizedVertices.pop_back();
			return normalizedVertices;
		}
	}
	return normalizedVertices;
}
int b2Separator::Validate(std::vector<b2Vec2> vertices)
{
	int listSize = vertices.size();
	int ret = 0;
	bool fl2 = false;
	for(int i = 0; i < listSize; i++)
	{
		int i2 = (i < listSize -1) ? i+1 : 0;
		int i3 = (i > 0) ? i-1 : listSize - 1;
		bool fl = false;
		for(int j = 0; j < listSize; j++)
		{
			if(j != i && j != i2)
			{
				if(!fl)
				{
					float d = det(vertices[i].x,vertices[i].y,vertices[i2].x,vertices[i2].y,vertices[j].x,vertices[j].y);
					if(d > 0) fl = true;
				}
				if(j != i3)
				{
					int j2 = (j < listSize - 1) ? j+1: 0 ;
					b2Vec2* vec = hitSegment(vertices[i].x,vertices[i].y,vertices[i2].x,vertices[i2].y,vertices[j].x,vertices[j].y,vertices[j2].x,vertices[j2].y);
					if(vec != NULL)
					{
						ret = 1;
					}
				}
			}
		}
		if(!fl) fl2 = true;
	}
	if(fl2)
	{
		if(ret == 1) ret =3;
		else ret = 2;
	}
	return ret;
}
std::vector<std::vector<b2Vec2>> b2Separator::calcShapes(std::vector<b2Vec2> verticesVec)
{
	std::vector<std::vector<b2Vec2>> separations; 
	std::queue<std::vector<b2Vec2>> queue;
	queue.push(verticesVec);	
	
	bool isConvex;
	while(!queue.empty())
	{
		std::vector<b2Vec2> list = queue.front();
		isConvex = true;
		int listSize = list.size();
		for(int i1 = 0 ; i1 < listSize; i1++)
		{
			int i2 = (i1 < listSize -1 ? i1+1:i1+1-listSize);
			int i3 = (i1 < listSize -2 ? i1+2:i1+2-listSize);

			b2Vec2 p1 = list.at(i1);
			b2Vec2 p2 = list.at(i2);
			b2Vec2 p3 = list.at(i3); 

			float result = det(p1.x,p1.y,p2.x,p2.y,p3.x,p3.y);
			if(result < 0)
			{
				isConvex = false;
				float minLen = FLT_MAX;
				int j1;
				int j2;
				int h = 0;
				int k = 0;
				b2Vec2 v1 = b2Vec2_zero;
				b2Vec2 v2 = b2Vec2_zero;
				b2Vec2 hitV = b2Vec2_zero;
				for(int j1 = 0; j1 < listSize; j1++)
				{
					if(j1 != i1 && j1 != i2)
					{
						j2 = (j1 < listSize-1 ? j1+1 : 0);
						v1 = list.at(j1);
						v2 = list.at(j2);
						b2Vec2* v = hitRay(p1.x,p1.y,p2.x,p2.y,v1.x,v1.y,v2.x,v2.y);
						if(v != NULL)
						{
							float dx = p2.x - v->x;
							float dy = p2.y - v->y;
							float t = dx*dx + dy*dy;

							if(t < minLen)
							{
								h = j1;
								k = j2;
								hitV = *v;
								minLen = t;
							}
						}

					}
				}
				if(minLen == FLT_MAX)
				{
					err();
				}
				std::vector<b2Vec2> vec1;
				std::vector<b2Vec2> vec2;
				j1 = h;
				j2 = k;

				if(!pointsMatch(hitV.x,hitV.y,v2.x,v2.y)) vec1.push_back(hitV);
				if(!pointsMatch(hitV.x,hitV.y,v1.x,v1.y)) vec2.push_back(hitV);

				h = -1;
				k = i1;
				while (true)
				{
					if(k != j2) vec1.push_back(list.at(k));
					else
					{
						if( h < 0 || h >= listSize)
						{
							err();
						}
						else
						{
							if(!isOnSegment(v2.x,v2.y,list.at(h).x,list.at(h).y,p1.x,p1.y)) vec1.push_back(list.at(k));
							break;
						}
					}
					h = k;
					if(k - 1 < 0) k = listSize -1;
					else k--;
				}
				std::vector<b2Vec2>::iterator start = vec1.begin();
				std::vector<b2Vec2>::iterator end = vec1.end();
				reverse(start,end);

				h = -1;
				k = i2;
				while(true)
				{
					if(k != j1)
					{
						vec2.push_back(list.at(k));
					}
					else
					{
						if(h < 0 || h >= listSize)
						{
							err();
						}else
						{
							if(k == j1 && !isOnSegment(v1.x,v1.y,list.at(h).x,list.at(h).y,p2.x,p2.y)) vec2.push_back(list.at(k));
							break;
						}
					}
					h = k;
					if(k + 1 > listSize-1) k = 0;
					else k++;
				}
				queue.push(vec1);
				queue.push(vec2);
				queue.pop();
				break;
			}
		}
		
		if(isConvex){
			separations.push_back(queue.front());
			queue.pop();
		}
	}
	return separations;
}
b2Vec2* b2Separator::hitRay(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4)
{
	float t1 = x3 - x1;
	float t2 = y3 - y1;
	float t3 = x2 - x1;
	float t4 = y2 - y1;
	float t5 = x4 - x3;
	float t6 = y4 - y3;
	float t7 = t4*t5 - t3*t6;
	float a = (t5*t2 - t6*t1)/t7;
	int px = x1 + a * t3;
	int py = y1 + a * t4;
	bool b1 = isOnSegment(x2,y2,x1,y1,px,py);
	bool b2 = isOnSegment(px,py,x3,y3,x4,y4);
	if(b1 && b2)
		return  new b2Vec2(px,py);
	return NULL;
}
b2Vec2* b2Separator::hitSegment(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4)
{
	float t1 = x3 - x1;
	float t2 = y3 - y1;
	float t3 = x2 - x1;
	float t4 = y2 - y1;
	float t5 = x4 - x3;
	float t6 = y4 - y3;
	float t7 = t4*t5 - t3*t6;
	float a = (t5*t2 - t6*t1)/t7;
	float px = x1 + a * t3;
	float py = y1 + a * t4;
	bool b1 = isOnSegment(px,py,x1,y1,x2,y2);
	bool b2 = isOnSegment(px,py,x3,y3,x4,y4);
	if(b1 && b2)
		return new b2Vec2(px,py);
	return NULL;
}
bool b2Separator::isOnSegment(float px,float py,float x1,float y1,float x2,float y2)
{
	bool b1 =( (x1 + 0.1 >= px && px >= x2-0.1) || (x1 - 0.1 <= px && px <= x2 + 0.1));
	bool b2 =( (y1 + 0.1 >= py && py >= y2-0.1) || (y1 - 0.1 <= py && py <= y2 + 0.1));
	return (b1 && b2 && isOnLine(px,py,x1,y1,x2,y2));
}
bool b2Separator::pointsMatch(float x1,float y1,float x2,float y2)
{
	float dx = ( x2 >= x1 ? x2-x1:x1-x2);
	float dy = ( y2 >= y1 ? y2-y1:y1-y2);
	return (dx < 0.1 && dy < 0.1);
}
bool b2Separator::isOnLine (float px,float py,float x1,float y1,float x2,float y2)
{
	if(x2 - x1 > 0.1 || x1 - x2 > 0.1)
	{
		float a = (y2 - y1) / (x2 - x1);
		float possibleY = a*(px - x1) + y1;
		float diff = (possibleY > py) ? possibleY - py:py-possibleY;
		return (diff < 0.1);
	}
	return px - x1 < 0.1 || x1 - px < 0.1;
}
float b2Separator::det(float x1,float y1,float x2,float y2,float x3,float y3)
{
	return x1*y2 + x2* y3 + x3*y1 - y1*x2 - y2*x3 - y3*x1; 

}