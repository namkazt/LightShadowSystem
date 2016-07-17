#ifndef _LIGHT_SOURCE_H_
#define _LIGHT_SOURCE_H_

#include "cocos2d.h"

class LightSource
{
private:
	int												_idx = -1;
	std::string										_idStr = "";

	cocos2d::Vec2									_lightPosition = cocos2d::Vec2(0,0);
	cocos2d::Vec4									_lightColor = cocos2d::Vec4(1,1,1,1);
	float											_range = 200.f;

	float											_luminance = 0.4f;

	cocos2d::GLProgramState*						_programState = NULL;

	std::map<std::string, cocos2d::Uniform>			_uniformMap;
	std::map<std::string, cocos2d::UniformValue>	_uniformValueMap;
public:

	LightSource(int index, cocos2d::GLProgramState* glProgramState);
	static LightSource*								create(int index, cocos2d::GLProgramState* glProgramState);
	//==================================================================================
	// GET
	//==================================================================================
	cocos2d::Vec2									getPosition() { return _lightPosition; };
	cocos2d::Vec4									getGLColor() { return _lightColor; };
	float											getRange() { return _range; };
	float											getLuminance() { return _luminance; };
	//==================================================================================
	// SET
	//==================================================================================
	void											setPosition(cocos2d::Vec2 pos);
	void											setPosition(int x, int y);
	void											setColor(float r, float g, float b, float a);
	void											setColor(const cocos2d::Vec4& glColor);
	void											setColor(const cocos2d::Color4F& ccColor);
	void											setColor(const cocos2d::Color4B& ccColor);
	void											setRange(float range);
	void											setLuminance(float luminance);
	//==================================================================================
	// UPDATE
	//==================================================================================
	void											updateAllUniform();
	void											updateGLPosition();
	void											updateGLColor();
	void											updateGLRange();
	void											updateGLLuminance();
};
#endif /* _LIGHT_SOURCE_H_ */
