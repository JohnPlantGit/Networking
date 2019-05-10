#pragma once
#include <glm/ext.hpp>
#include <Renderer2D.h>
class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Update() {};
	virtual void Draw(aie::Renderer2D* renderer) {};

protected:
	glm::vec2 m_position;
};

