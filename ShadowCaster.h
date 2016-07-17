#ifndef _SHADOW_CASTER_H_
#define _SHADOW_CASTER_H_

#include "cocos2d.h"


class ShadowCaster : public cocos2d::Node
{
private:
	cocos2d::Sprite*														_sprite;

	std::vector<cocos2d::Vec2>												_polyData;
	int																		_polyCount;
	int																		_position;
	int																		_type;
public:
	static ShadowCaster*													create(cocos2d::Sprite* sprite);

	bool init(cocos2d::Sprite* sprite);
};
#endif /* _SHADOW_CASTER_H_ */
