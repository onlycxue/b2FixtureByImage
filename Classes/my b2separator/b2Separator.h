/*
* Convex Separator for Box2D Flash
*
* This class has been written by Antoan Angelov. 
* It is designed to work with Erin Catto's Box2D physics library.
*
* Everybody can use this software for any purpose, under two restrictions:
* 1. You cannot claim that you wrote this software.
* 2. You can not remove or alter this notice.
*
*/

#include <vector>
#include <queue>
#include "Box2D\Box2D.h"

class b2Separator
{
public:
	b2Separator(){};
	void Separator(b2Body* body,b2FixtureDef fixtureDef,std::vector<b2Vec2> verticesVec,int scale = 30);
	int Validate(std::vector<b2Vec2> vertices);

private:
	std::vector<b2Vec2> removeZeroLengthEdges(std::vector<b2Vec2> normalizedVertices);
	std::vector<std::vector<b2Vec2>> calcShapes(std::vector<b2Vec2> verticesVec);
	b2Vec2* convertTob2Vec2Ptr(std::vector<b2Vec2> array);
	b2Vec2* hitRay(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4);
	b2Vec2* hitSegment(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4);
	bool isOnSegment(float px,float py,float x1,float y1,float x2,float y2);
	bool pointsMatch(float x1,float y1,float x2,float y2);
	bool isOnLine (float px,float py,float x1,float y1,float x2,float y2);
	float det(float x1,float y1,float x2,float y2,float x3,float y3);
	void err()
	{
		assert("A problem has occurred.Use the Validate() method to see where the problem is. ");
	}
	~b2Separator();

private:

};