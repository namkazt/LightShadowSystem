#include "DynamicShadowScene.h"

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC))
#include "CCIMGUI.h"
#include "imgui_internal.h"
#include "imgui_extend.h"
#include <iomanip>
#include <ctime>
#endif
#include <Libs/dynamicshadow/LightSource.h>


USING_NS_CC;

Scene* DynamicShadowScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = DynamicShadowScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool DynamicShadowScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	Size visibleSize = Director::getInstance()->getVisibleSize();


	auto background = Sprite::create("res/bg2.png");
	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(background);


	lightCaster = RenderTexture::create(visibleSize.width, visibleSize.height);
	lightCaster->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	auto glprogramLight = GLProgram::createWithFilenames("res/shaders/p.vsh", "res/shaders/dynamicLight.psh");
	auto stateLight = GLProgramState::getOrCreateWithGLProgram(glprogramLight);
	lightCaster->getSprite()->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
	//---------------------------------------------------------------
	stateLight->setUniformVec2("u_resolution", Vec2(visibleSize.width, visibleSize.height));
	//---------------------------------------------------------------
	lightCaster->getSprite()->setGLProgramState(stateLight);
	this->addChild(lightCaster, 10);
	// Polygon
	stateLight->setUniformVec2("ccl_u_polyData[0].p", Vec2(600, 250));  // points
	stateLight->setUniformVec2("ccl_u_polyData[1].p", Vec2(650, 280));
	stateLight->setUniformVec2("ccl_u_polyData[2].p", Vec2(680, 240));
	stateLight->setUniformInt("ccl_u_shadowData[0].polyCount", 3);
	stateLight->setUniformInt("ccl_u_shadowData[0].type", 1); // polygon triangle
	stateLight->setUniformInt("ccl_u_shadowData[0].pos", 0);
	// Cirlce
	stateLight->setUniformVec2("ccl_u_polyData[3].p", Vec2(100, 250));  // position
	stateLight->setUniformVec2("ccl_u_polyData[4].p", Vec2(40, 0));  // size
	stateLight->setUniformInt("ccl_u_shadowData[1].polyCount", 2);
	stateLight->setUniformInt("ccl_u_shadowData[1].type", 2); // circle
	stateLight->setUniformInt("ccl_u_shadowData[1].pos", 3); // next in array
	// Box
	stateLight->setUniformVec2("ccl_u_polyData[5].p", Vec2(300, 250));  // position
	stateLight->setUniformVec2("ccl_u_polyData[6].p", Vec2(40, 40));  // size
	stateLight->setUniformVec2("ccl_u_polyData[7].p", Vec2(3.0, 0));  // corner 
	stateLight->setUniformInt("ccl_u_shadowData[2].polyCount", 3);
	stateLight->setUniformInt("ccl_u_shadowData[2].type", 3); // circle
	stateLight->setUniformInt("ccl_u_shadowData[2].pos", 5); // next in array

	stateLight->setUniformInt("u_shadowDataCount", 3);


	LightSource* light1 = LightSource::create(0, stateLight);
	light1->setPosition(300, 400);
	light1->setColor(Color4F::GREEN);
	light1->setRange(270.f);
	light1->setLuminance(0.4);
	stateLight->setUniformInt("u_lightCount", 1);




	//////////////////////////////////////////////////////////////////////
	// Setup listener
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto mouse = EventListenerMouse::create();
	mouse->onMouseMove = [=](Event* event)
	{
		Point pos = ((EventMouse*)event)->getLocationInView();
		light1->setPosition(pos.x, pos.y);
	};
	dispatcher->addEventListenerWithSceneGraphPriority(mouse, this);

    return true;
}
