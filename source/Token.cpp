#include "Token.hpp"

Token::Token(int value, int _x, int _y, SDL_Helper * _helper, char * _sprite, int _currentFrameX, int _currentFrameY, int _sizePerFrameX, int _sizePerFrameY, Settings * _settings) : Sprite (_x, _y, _helper, _sprite, 1, 1, _sizePerFrameX, _sizePerFrameY, _currentFrameX, _currentFrameY, true, false, 1)
{
	this->m_value = value;
	this->m_isMoving = false;
	this->m_dx = 0;
	this->m_dy = 0;
}

Token::~Token()
{

}

bool Token::IsMoving()
{
	return this->m_isMoving;
}

void Token::SetMoving(bool _value)
{
	this->m_isMoving = _value;
}

void Token::SetDirection(int _x, int _y)
{
	this->m_dx = _x;
	this->m_dy = _y;
}

int Token::GetValue()
{
	return this->m_value;
}

void Token::SetValue(int _value)
{
	this->m_value = _value;
}