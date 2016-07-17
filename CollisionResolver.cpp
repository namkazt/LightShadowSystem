#include "CollisionResolver.h"

USING_NS_CC;

bool CollisionResolver::isIntersectPoly(std::vector<cocos2d::Vec2>& polygonA, std::vector<cocos2d::Vec2>& polygonB)
{
	std::vector<Vec2> axis;
	for (int i = 0, j = polygonA.size() - 1; i < polygonA.size(); j = i, i++)
	{
		Vec2 e = polygonA[i] - polygonA[j];
		Vec2 n = Vec2(-e.y, e.x);
		axis.push_back(n);
		if (axisSeparatePolygons(&n, polygonA, polygonB))
			return false;
	}
	for (int i = 0, j = polygonB.size() - 1; i < polygonB.size(); j = i, i++)
	{
		Vec2 e = polygonB[i] - polygonB[j];
		Vec2 n = Vec2(-e.y, e.x);
		axis.push_back(n);
		if (axisSeparatePolygons(&n, polygonA, polygonB))
			return false;
	}
	return true;
}

std::vector<cocos2d::Vec2> CollisionResolver::setPolygonPosition(std::vector<cocos2d::Vec2>& polygon, const Vec2& position)
{
	std::vector<cocos2d::Vec2> result(polygon.size());
	for (int i = 0; i < polygon.size(); i++)
	{
		result[i] = polygon[i];
		result[i].add(position);
	}
	return result;
}

bool CollisionResolver::isConvexPoly(std::vector<cocos2d::Vec2>& polygon)
{
	if (polygon.size() < 4) return true;
	bool sign = false;
	int n = polygon.size();
	for (int i = 0; i < n; i++)
	{
		Vec2 d1 = polygon[idx(i+2,n)] - polygon[idx(i+1,n)];
		Vec2 d2 = polygon[idx(i,n)] - polygon[idx(i+1,n)];
		double cross = d1.cross(d2);
		if(i == 0)
			sign = cross > 0;
		else
		{
			if (sign != (cross > 0))
				return false;
		}
	}
	return true;
}

std::vector<cocos2d::Vec2> CollisionResolver::rotationPolygon(std::vector<cocos2d::Vec2>& polygon, float degree)
{
	float radian = degreeToRadian(degree);
	std::vector<cocos2d::Vec2> result(polygon.size());
	for (int i = 0; i < polygon.size(); i++)
	{
		result[i].x = polygon[i].x * std::cos(radian) - polygon[i].y * std::sin(radian);
		result[i].y = polygon[i].x * std::sin(radian) + polygon[i].y * std::cos(radian);
	}
	return result;
}

bool CollisionResolver::axisSeparatePolygons(cocos2d::Vec2* axis, std::vector<cocos2d::Vec2>& polygonA, std::vector<cocos2d::Vec2>& polygonB)
{
	float minA, maxA;
	float minB, maxB;

	calculateInterval(*axis, polygonA, &minA, &maxA);
	calculateInterval(*axis, polygonB, &minB, &maxB);

	if (minA > maxB || minB > maxA)
		return true;
	// find the interval overlap
	float d0 = maxA - minB;
	float d1 = maxB - minA;
	float depth = (d0 < d1) ? d0 : d1;

	// convert the separation axis into a push vector (re-normalise 
	// the axis and multiply by interval overlap) 
	float axisLengthSquared = (*axis).dot(*axis);
	*axis = *axis * depth / axisLengthSquared;
	return false;
}

void CollisionResolver::calculateInterval(const cocos2d::Vec2& axis, std::vector<cocos2d::Vec2>& polygon, float* min, float* max)
{
	float d = axis.dot(polygon[0]);
	*min = d;
	*max = d;
	for (int i = 0; i < polygon.size(); i++)
	{
		d = axis.dot(polygon[i]);
		if (d < *min) *min = d;
		else if (d > *max) *max = d;
	}
}

bool CollisionResolver::floatEqual(float v1, float v2)
{
	return std::abs(v1 - v2) <= MATH_EPSILON;
}

int CollisionResolver::idx(int _idx, int size)
{
	return _idx < 0 ? size - (-_idx % size) : _idx % size;
}

std::vector<std::vector<cocos2d::Vec2>> CollisionResolver::convexPartition(std::vector<cocos2d::Vec2>& polygon)
{
	//We force it to CCW as it is a precondition in this algorithm.
	forceCounterClockWise(&polygon);
	//----------------------------------------------------
	std::vector<std::vector<cocos2d::Vec2>> result;
	float d, lowerDist, upperDist;
	Vec2 p;
	Vec2 lowerInt;
	Vec2 upperInt;
	int lowerIdx = 0, upperIdx = 0;
	std::vector<cocos2d::Vec2> lowerPoly, upperPoly;
	int n = polygon.size();
	//----------------------------------------------------
	for (int i = 0; i < n; i++)
	{
		if(reflex(i, &polygon))
		{
			lowerDist = upperDist = std::numeric_limits<float>::max();
			for (int j = 0; j < n; j++)
			{
				// if line intersects with an edge
				if(left(polygon[idx(i-1,n)], polygon[idx(i,n)], polygon[idx(j,n)]) &&
					rightOn(polygon[idx(i - 1, n)], polygon[idx(i, n)], polygon[idx(j-1, n)]))
				{
					p = lineIntersect(polygon[idx(i - 1, n)], polygon[idx(i, n)], polygon[idx(j, n)], polygon[idx(j - 1, n)]);
					if(right(polygon[idx(i + 1, n)], polygon[idx(i, n)], p))
					{
						d = polygon[idx(i, n)].distanceSquared(p);
						if(d < lowerDist)
						{
							lowerDist = d;
							lowerInt = p;
							lowerIdx = j;
						}
					}
				}
				if (left(polygon[idx(i+1, n)], polygon[idx(i, n)], polygon[idx(j+1, n)]) &&
					rightOn(polygon[idx(i+1, n)], polygon[idx(i, n)], polygon[idx(j, n)]))
				{
					p = lineIntersect(polygon[idx(i+1, n)], polygon[idx(i, n)], polygon[idx(j, n)], polygon[idx(j+1, n)]);
					if (left(polygon[idx(i-1, n)], polygon[idx(i, n)], p))
					{
						d = polygon[idx(i, n)].distanceSquared(p);
						if (d < upperDist)
						{
							upperDist = d;
							upperIdx = j;
							upperInt = p;
						}
					}
				}
			}
			// if there are no vertices to connect to, choose a point in the middle
			if(lowerIdx == (upperIdx + 1)%n)
			{
				Vec2 sp = ((lowerInt + upperInt) / 2);
				lowerPoly = copyPoly(i, upperIdx, &polygon);
				lowerPoly.push_back(sp);
				upperPoly = copyPoly(lowerIdx, i, &polygon);
				upperPoly.push_back(sp);
			}else
			{
				double highestScore = 0, bestIndex = lowerIdx;
				while (upperIdx < lowerIdx) upperIdx += n;
				for (int j = lowerIdx; j <= upperIdx; ++j)
				{
					if(isCanSee(i,j, &polygon))
					{
						double score = 1 / polygon[idx(i, n)].distanceSquared(polygon[idx(j, n)]) + 1;
						if(reflex(j, &polygon))
						{
							if(rightOn(polygon[idx(j-1, n)], polygon[idx(j, n)], polygon[idx(i, n)]) &&
								leftOn(polygon[idx(j+1, n)], polygon[idx(j, n)], polygon[idx(i, n)]))
							{
								score += 3;
							}else
							{
								score += 2;
							}
						}else
						{
							score += 1;
						}
						if(score >= highestScore)
						{
							bestIndex = j;
							highestScore = score;
						}
					}
				}
				lowerPoly = copyPoly(i, (int)bestIndex, &polygon);
				upperPoly = copyPoly((int)bestIndex, i, &polygon);
			}
			// check if lower and upper is convex or concave
			if (isConvexPoly(lowerPoly)) result.push_back(lowerPoly);
			else
			{
				std::vector<std::vector<cocos2d::Vec2>> lowerDecompose = convexPartition(lowerPoly);
				result.insert(result.end(), lowerDecompose.begin(), lowerDecompose.end());
			}
			if (isConvexPoly(upperPoly)) result.push_back(upperPoly);
			else
			{
				std::vector<std::vector<cocos2d::Vec2>> upperDecompose = convexPartition(upperPoly);
				result.insert(result.end(), upperDecompose.begin(), upperDecompose.end());
			}
			return result;
		}
	}

	// polygon is already convex
	result.push_back(polygon);
	return result;
}

void CollisionResolver::forceCounterClockWise(std::vector<cocos2d::Vec2>* polygon)
{
	if (polygon->size() < 3) return;
	if(getSignedArea(*polygon) <= 0.0f)
	{
		polygon->reserve(polygon->size());
	}
}

float CollisionResolver::getSignedArea(std::vector<cocos2d::Vec2>& polygon)
{
	int i;
	float area = 0;
	for (i = 0; i < polygon.size(); i++)
	{
		int j = (i + 1) % polygon.size();
		area += polygon[i].cross(polygon[j]);
	}
	area /= 2.0f;
	return area;
}

std::vector<cocos2d::Vec2> CollisionResolver::copyPoly(int i, int j, std::vector<cocos2d::Vec2>* poly)
{
	std::vector<cocos2d::Vec2> result;
	while (j < i) j += poly->size();
	for (; i <= j; ++i)
	{
		result.push_back(poly->at(i % poly->size()));
	}
	return result;
}

bool CollisionResolver::isCanSee(int i, int j, std::vector<cocos2d::Vec2>* polygon)
{
	int n = polygon->size();
	if(reflex(i, polygon))
	{
		if (leftOn(polygon->at(idx(i, n)), polygon->at(idx(i-1, n)), polygon->at(idx(j, n))) &&
			rightOn(polygon->at(idx(i, n)), polygon->at(idx(i+1, n)), polygon->at(idx(j, n)))) return false;
	}else
	{
		if (rightOn(polygon->at(idx(i, n)), polygon->at(idx(i + 1, n)), polygon->at(idx(j, n))) ||
			leftOn(polygon->at(idx(i, n)), polygon->at(idx(i - 1, n)), polygon->at(idx(j, n)))) return false;
	}
	if (reflex(j, polygon))
	{
		if (leftOn(polygon->at(idx(j, n)), polygon->at(idx(j-1, n)), polygon->at(idx(i, n))) &&
			rightOn(polygon->at(idx(j, n)), polygon->at(idx(j+1, n)), polygon->at(idx(i, n)))) return false;
	}
	else
	{
		if (rightOn(polygon->at(idx(j, n)), polygon->at(idx(j+1, n)), polygon->at(idx(i, n))) ||
			leftOn(polygon->at(idx(j, n)), polygon->at(idx(j-1, n)), polygon->at(idx(i, n)))) return false;
	}

	for (int k = 0; k < n; ++k)
	{
		if((k + 1)%n == i || k == i || (k + 1) % n == j || k == j)
			continue;
		Vec2 intersectionPoint;
		if(lineIntersectCross(polygon->at(idx(i, n)), polygon->at(idx(j, n)), polygon->at(idx(k, n)), polygon->at(idx(k+1, n)), &intersectionPoint))
		{
			return false;
		}
	}
	return true;
}

bool CollisionResolver::reflex(int i, std::vector<cocos2d::Vec2>* polygon)
{
	return right(i, polygon);
}

bool CollisionResolver::right(int i, std::vector<cocos2d::Vec2>* polygon)
{
	int n = polygon->size();
	return right(polygon->at(idx(i-1, n)), polygon->at(idx(i, n)), polygon->at(idx(i + 1, n)));
}

bool CollisionResolver::right(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c)
{
	return getArea(a, b, c) < 0;
}

bool CollisionResolver::rightOn(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c)
{
	return getArea(a, b, c) <= 0;
}

bool CollisionResolver::left(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c)
{
	return getArea(a, b, c) > 0;
}

bool CollisionResolver::leftOn(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c)
{
	return getArea(a, b, c) >= 0;
}

float CollisionResolver::getArea(const cocos2d::Vec2& a, const cocos2d::Vec2& b, const cocos2d::Vec2& c)
{
	return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
}

bool CollisionResolver::lineIntersectCross(const cocos2d::Vec2& a0, const cocos2d::Vec2& a1, const cocos2d::Vec2& b0, const cocos2d::Vec2& b1, cocos2d::Vec2* intersectionPoint)
{
	*intersectionPoint = Vec2::ZERO;
	if (a0 == b0 || a0 == b1 || a1 == b0 || a1 == b1) return false;

	float x1 = a0.x, y1 = a0.y, x2 = a1.x, y2 = a1.y;
	float x3 = b0.x, y3 = b0.y, x4 = b1.x, y4 = b1.y;
	//AABB early exit
	if (std::max(x1, x2) < std::min(x3, x4) || std::max(x3, x4) < std::min(x1, x2)) return false;
	if (std::max(y1, y2) < std::min(y3, y4) || std::max(y3, y4) < std::min(y1, y2)) return false;

	float ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3));
	float ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3));
	float denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
	if (std::abs(denom) < MATH_EPSILON) return false;
	ua /= denom;
	ub /= denom;
	if ((0 < ua) && (ua < 1) && (0 < ub) && (ub < 1))
	{
		intersectionPoint->x = (x1 + ua * (x2 - x1));
		intersectionPoint->y = (y1 + ua * (y2 - y1));
		return true;
	}
	return false;
}

cocos2d::Vec2& CollisionResolver::lineIntersect(const cocos2d::Vec2& p1, const cocos2d::Vec2& p2, const cocos2d::Vec2& q1, const cocos2d::Vec2& q2)
{
	Vec2 i = Vec2::ZERO;
	float a1 = p2.y - p1.y;
	float b1 = p1.x - p2.x;
	float c1 = a1 * p1.x + b1 * p1.y;
	float a2 = q2.y - q1.y;
	float b2 = q1.x - q2.x;
	float c2 = a2* q1.x + b2 * q1.y;
	float det = a1 * b2 - a2 * b1;
	if(!floatEqual(det, 0))
	{
		i.x = (b2 * c1 - b1 * c2) / det;
		i.y = (a1 * c2 - a2 * c1) / det;
	}
	return i;
}

std::vector<std::vector<cocos2d::Vec2>> CollisionResolver::validatePolygon(std::vector<cocos2d::Vec2>& polygon)
{
	std::vector<std::vector<cocos2d::Vec2>> result;
	if(isConvexPoly(polygon))
		result.push_back(polygon);
	else
	{
		result = convexPartition(polygon);
	}
	return result;
}

bool CollisionResolver::isPointIntersectPoly(const cocos2d::Vec2& point, std::vector<cocos2d::Vec2>& polygon)
{
	int i, j, c = 0;
	for (j = polygon.size()-1, i = 0; i < polygon.size(); j = i++)
	{
		if( ((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
			(point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y)/(polygon[j].y - polygon[i].y) + polygon[i].x))
		{
			c = !c;
		}
	}
	return c;
}