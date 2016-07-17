#include "LightManager.h"

USING_NS_CC;

LightManager* LightManager::create()
{
	LightManager* pRef = new (std::nothrow) LightManager();
	pRef->init();
	return pRef;
}

void LightManager::init()
{
	
}