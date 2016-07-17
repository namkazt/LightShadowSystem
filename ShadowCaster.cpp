#include "ShadowCaster.h"

USING_NS_CC;

ShadowCaster* ShadowCaster::create(cocos2d::Sprite* sprite)
{
	ShadowCaster* ref = new (std::nothrow) ShadowCaster();
	if(ref->init(sprite))
	{
		ref->autorelease();
		return ref;
	}else
	{
		CC_SAFE_DELETE(ref);
		ref = nullptr;
		return nullptr;
	}
}

bool ShadowCaster::init(cocos2d::Sprite* sprite)
{
	if (!Node::init()) return false;
	// set sprite
	_sprite = sprite;
	//--- init default polygon
	_type = 3; // mean Box
	_position = 0;
	_polyCount = 0;
	return true;
}