#pragma once
#include <glm/ext.hpp>
#include <Renderer2D.h>
class GameObject
{
public:
	GameObject();
	~GameObject();

	/*
		@brief Handles any updates needed for the game object
	*/
	virtual void Update() {};
	/*
		@brief Renders the gameobject
	*/
	virtual void Draw(aie::Renderer2D* renderer) {};

protected:
	glm::vec2 m_position;
};

