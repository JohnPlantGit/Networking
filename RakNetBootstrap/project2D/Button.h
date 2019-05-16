#pragma once
#include "GameObject.h"
class Button : GameObject
{
public:
	/*
		@brief Creates a button 
		@param The position of the button
		@param The size of the button
	*/
	Button(glm::vec2 pos, glm::vec2 size);
	~Button();

	/*
		@brief Draws the button as a box 
	*/
	virtual void Draw(aie::Renderer2D* renderer);

	/*
		@brief Checks if a point is inside the button's extents
		@return if the point is inside the button
	*/
	bool ContainsPoint(glm::vec2 point);

	/*
		@brief Gets the position of the button
		@return the vector position of the button
	*/
	glm::vec2 GetPosition() { return m_position; }

private:
	/*
		@brief Gets the min x/y of the button
		@return The bottom left corner of the button
	*/
	glm::vec2 GetMin();
	/*
		@brief Gets the max x/y of the button
		@return The top right corner of the button
	*/
	glm::vec2 GetMax();

	glm::vec2 m_size;

};

