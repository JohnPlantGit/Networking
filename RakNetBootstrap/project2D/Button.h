#pragma once
#include "GameObject.h"
class Button : GameObject
{
public:
	Button(glm::vec2 pos, glm::vec2 size);
	~Button();

	virtual void Update();
	virtual void Draw(aie::Renderer2D* renderer);

	bool ContainsPoint(glm::vec2 point);

	glm::vec2 GetPosition() { return m_position; }

private:
	glm::vec2 GetMin();
	glm::vec2 GetMax();

	glm::vec2 m_size;

};

