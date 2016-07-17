#ifndef __DYNAMIC_SHADOW_SCENE_H__
#define __DYNAMIC_SHADOW_SCENE_H__

#include "cocos2d.h"

#include "LightManager.h"

class DynamicShadowScene : public cocos2d::Layer
{

	cocos2d::RenderTexture*								lightCaster;

	LightManager*										_lightManager;

public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(DynamicShadowScene);
};

#endif // __DYNAMIC_SHADOW_SCENE_H__
