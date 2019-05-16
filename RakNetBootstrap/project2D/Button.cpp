#include "Button.h"
#include <Input.h>

Button::Button(glm::vec2 pos, glm::vec2 size)
{
	m_position = pos;
	m_size = size;
}

Button::~Button()
{
}

void Button::Draw(aie::Renderer2D* renderer)
{
	renderer->drawBox(m_position.x, m_position.y, m_size.x, m_size.y);
}

glm::vec2 Button::GetMin()
{
	glm::vec2 min;
	min.x = m_position.x - (m_size.x / 2);
	min.y = m_position.y - (m_size.y / 2);
	return min;
}

glm::vec2 Button::GetMax()
{
	glm::vec2 max;
	max.x = m_position.x + (m_size.x / 2);
	max.y = m_position.y + (m_size.y / 2);
	return max;
}

bool Button::ContainsPoint(glm::vec2 point)
{
	glm::vec2 min = GetMin();
	glm::vec2 max = GetMax();
	return (point.x > min.x && point.y > min.y && point.x < max.x && point.y < max.y);
}