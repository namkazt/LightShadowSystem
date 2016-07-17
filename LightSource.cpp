#include "LightSource.h"

USING_NS_CC;


LightSource::LightSource(int index, cocos2d::GLProgramState* glProgramState)
{
	//--- create id string to pass to shader
	_idx = index;
	char id[50] = "";
	sprintf(id, "ccl_u_lights[%i]", _idx);
	_idStr = id;
	//-----------------------------
	_programState = glProgramState;
}


LightSource* LightSource::create(int index, cocos2d::GLProgramState* glProgramState)
{
	LightSource* pRef = new (std::nothrow) LightSource(index, glProgramState);
	return pRef;
}

void LightSource::setPosition(cocos2d::Vec2 pos)
{
	setPosition(pos.x, pos.y);
}

void LightSource::setPosition(int x, int y)
{
	_lightPosition.x = x;
	_lightPosition.y = y;
	updateGLPosition();
}

void LightSource::setColor(float r, float g, float b, float a)
{
	_lightColor.x = r;
	_lightColor.y = g;
	_lightColor.z = b;
	_lightColor.w = a;
	updateGLColor();
}

void LightSource::setColor(const cocos2d::Vec4& glColor)
{
	setColor(glColor.x, glColor.y, glColor.z, glColor.w);
}

void LightSource::setColor(const cocos2d::Color4F& ccColor)
{
	setColor(ccColor.r, ccColor.g, ccColor.b, ccColor.a);
}

void LightSource::setColor(const cocos2d::Color4B& ccColor)
{
	setColor(ccColor.r / 255.f, ccColor.g / 255.f, ccColor.b / 255.f, ccColor.a / 255.f);
}

void LightSource::setRange(float range)
{
	_range = range;
	updateGLRange();
}

void LightSource::setLuminance(float luminance)
{
	_luminance = luminance;
	updateGLLuminance();
}

void LightSource::updateAllUniform()
{
	updateGLPosition();
	updateGLColor();
	updateGLRange();
	updateGLLuminance();
}

void LightSource::updateGLPosition()
{
	if (_programState == NULL) return;
	_programState->setUniformVec2("ccl_u_lights[0].lightPos", _lightPosition);
}

void LightSource::updateGLColor()
{
	if (_programState == NULL) return;
	_programState->setUniformVec4("ccl_u_lights[0].lightCol", _lightColor);
}

void LightSource::updateGLRange()
{
	if (_programState == NULL) return;
	_programState->setUniformFloat("ccl_u_lights[0].range", _range);
}

void LightSource::updateGLLuminance()
{
	if (_programState == NULL) return;
	_programState->setUniformFloat("ccl_u_lights[0].luminance", _luminance);
}