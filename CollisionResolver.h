#ifndef __COLLISION_RESOLVER_H__
#define __COLLISION_RESOLVER_H__

#include "cocos2d.h"

/*
	validate polygon function using "Mark Bayazit's decomposition algorithm" short as "Bayazit Decomposer"
	this function took from "Farseer Physics"
*/

class CollisionResolver
{
private:
	static bool axisSeparatePolygons(cocos2d::Vec2* axis, std::vector<cocos2d::Vec2>& polygonA, std::vector<cocos2d::Vec2>& polygonB);
	static void calculateInterval(const cocos2d::Vec2& axis, std::vector<cocos2d::Vec2>& polygon, float* min, float* max);
	static bool floatEqual(float v1, float v2);
	static int idx(int idx, int size);
	//----------------------------------------------------
	// Bayazit Decomposer
	//----------------------------------------------------
	static std::vector<std::vector<cocos2d::Vec2>> convexPartition(std::vector<cocos2d::Vec2>& polygon);
	static void forceCounterClockWise(std::vector<cocos2d::Vec2>* polygon);
	static float getSignedArea(std::vector<cocos2d::Vec2>& polygon);
	static std::vector<cocos2d::Vec2> copyPoly(int i, int j, std::vector<cocos2d::Vec2>* poly);
	static bool isCanSee(int i, int j, std::vector<cocos2d::Vec2>* polygon);
	static bool reflex(int i, std::vector<cocos2d::Vec2>* polygon);
	static bool right(int i, std::vector<cocos2d::Vec2>* polygon);
	static bool right(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c);
	static bool rightOn(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c);
	static bool left(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c);
	static bool leftOn(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c);
	static float getArea(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c);
	static bool lineIntersectCross(const cocos2d::Vec2& a0, const cocos2d::Vec2& a1, const cocos2d::Vec2& b0, const cocos2d::Vec2&  b1, cocos2d::Vec2* intersectionPoint);
	static cocos2d::Vec2& lineIntersect(const cocos2d::Vec2& p1, const cocos2d::Vec2& p2, const cocos2d::Vec2& q1, const cocos2d::Vec2& q2);
public:
	/*
	Validate if polygon is not convex and auto decompose polygon from concave to convex
	return single vector of polygon if is convex
	Using: "Bayazit Decomposer" method
	*/
	static std::vector<std::vector<cocos2d::Vec2>> validatePolygon(std::vector<cocos2d::Vec2>& polygon);

	static bool isPointIntersectPoly(const cocos2d::Vec2& point, std::vector<cocos2d::Vec2>& polygon);
	
	static bool isIntersectPoly(std::vector<cocos2d::Vec2>& polygonA, std::vector<cocos2d::Vec2>& polygonB);
	
	static std::vector<cocos2d::Vec2> setPolygonPosition(std::vector<cocos2d::Vec2>& polygon, const cocos2d::Vec2& position);
	
	static bool isConvexPoly(std::vector<cocos2d::Vec2>& polygon);

	static std::vector<cocos2d::Vec2> rotationPolygon(std::vector<cocos2d::Vec2>& polygon, float degree);

	static float degreeToRadian(float degree) { return degree * M_PI / 180.0f; };
	static float radianToDegree(float radian) { return radian * 180.0f / M_PI; };
};

#endif // __COLLISION_RESOLVER_H__
