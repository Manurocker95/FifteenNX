#pragma once

#ifndef _TOKEN_HPP_
#define _TOKEN_HPP_

#include "Sprite.hpp"
#include "Settings.hpp"

class Token : public Sprite
{
private:
	bool m_isMoving;
	bool m_displacement;
	int  m_dx;
	int m_dy;
	int m_value;

public:

	~Token();
	Token(int value, int _x, int _y, SDL_Helper * _helper, char * _sprite, int _currentFrameX, int _currentFrameY, int _sizePerFrameX, int _sizePerFrameY, Settings * _settings);
	bool IsMoving();
	void SetMoving(bool _value);
	void SetDirection(int _x, int _y);
	int GetValue();
	void SetValue(int value);
};

#endif