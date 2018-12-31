/* This file is part of Manurocker95's Template!

this is made for my tutorial: https://gbatemp.net/threads/tutorial-setting-up-visual-studio-2017-environment-for-nintendo-switch-homebrew-development.525977/#post-8439059

Copyright (C) 2018/2019 Manuel Rodríguez Matesanz
>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
*/

#include "GameScreen.hpp"
#include "SceneManager.hpp"
#include "Filepaths.h"
#include "Colors.h"
#include <string>
#include <random>
#include <ctime>
#include <cstdlib>
#include <algorithm>

GameScreen::GameScreen(Settings * _settings) : Scene(_settings)
{
	this->m_settings = _settings;
	this->m_changeScene = false;
	this->m_tokenSize = _settings->m_tokenSize;
	this->m_steps = 0;
	this->m_seconds = 59;
	this->m_lastTime = 0;
	this->m_minutes = _settings->m_maxTime-1;
	this->m_currentTime = SDL_GetTicks();
	srand(time(NULL));
	
}

GameScreen::~GameScreen()
{
	this->m_music->End(this->m_helper);
	delete(this->m_music);

	this->m_buttonTapSFX->End(this->m_helper);
	delete(this->m_buttonTapSFX);

	this->m_background->End(this->m_helper);
	delete(this->m_background);

	this->m_timeText->End(this->m_helper);
	delete(this->m_timeText);

	this->m_stepsText->End(this->m_helper);
	delete(this->m_stepsText);

	this->m_resetButton->End(this->m_helper);
	delete(this->m_resetButton);

	for (auto & vector : m_tokens)
	{
		for (auto & token : vector)
		{
			token->End(this->m_helper);
			delete(token);
		}

		vector.clear();
	}
	this->m_tokens.clear();
	this->m_correct.clear();
}

void GameScreen::Start(SDL_Helper * helper)
{
	
	this->m_helper = helper;
	this->m_background = new Sprite(0, 0, helper, IMG_BACKGROUND, 1, 1, SWITCH_SCREEN_WIDTH, SWITCH_SCREEN_HEIGHT, 0, 0, false, false, 1);

	this->m_timeText = new Text(helper,std::string(SceneManager::Instance()->GetText("timeText")) + std::to_string(this->m_minutes) + std::string(" : ") + std::to_string(this->m_seconds), 925, 30, 45, true, FONT_PUZZLE, C_BLACK);
	this->m_stepsText = new Text(helper, std::string(SceneManager::Instance()->GetText("stepText")) +  std::to_string(this->m_steps), 30, 30, 45, true, FONT_PUZZLE, C_BLACK);
	this->m_resetButton = new Button(50, 420, helper, IMG_BTN_RESET, IMG_BTN_RESET, IMG_BTN_RESET, true, false, 1, 1, 208, 116, false, 0, 0);
	std::vector<Token *> vector;
	std::vector<int> int_vector;
	int value = 1;
	int _x = 0;
	int _y = 104;
	
	for (int i = 0; i < this->m_settings->m_yTokens; i++)
	{
		_x = 384;
	
		vector.clear();
		int_vector.clear();

		for (int j = 0; j < this->m_settings->m_xTokens; j++)
		{
			vector.push_back(new Token(value, _x, _y, helper, IMG_TOKENS, j, i, this->m_tokenSize, this->m_tokenSize, this->m_settings));
			
			int_vector.push_back(value);
			++value;
			
			_x += this->m_tokenSize;
		}
		_y += this->m_tokenSize;

		this->m_tokens.push_back(vector);
		this->m_correct.push_back(int_vector);
	}
	vector.clear();

	ShufflePuzzle();

	this->m_buttonTapSFX = new SfxSound(this->m_helper, SND_SFX_TAP, false, 2);
	this->m_music = new MusicSound(this->m_helper, SND_BGM_GAME, true, 1);
	this->m_music->Play(this->m_helper);
	
	if (this->m_settings->m_muted)
		this->m_helper->SDL_PauseMusic();

}

void GameScreen::Draw()
{
	this->m_background->Draw(this->m_helper);

	for (int y = 0; y < m_tokens.size(); y++)
	{
		for (int x = 0; x < m_tokens[y].size(); x++)
		{
			m_tokens[y][x]->Draw(this->m_helper);
		}
	}

	this->m_resetButton->Draw(this->m_helper);

	this->m_stepsText->Draw(this->m_helper);
	this->m_timeText->Draw(this->m_helper);
}

void GameScreen::Update()
{	
	if (this->m_changeScene)
		NextScene();
	else
	{
		this->m_currentTime = SDL_GetTicks();

		// each second
		if (this->m_currentTime > this->m_lastTime + 10000)
		{
			this->m_lastTime = this->m_currentTime;

			--this->m_seconds;

			if (this->m_seconds == 0)
			{
				--this->m_minutes;
				if (m_minutes <= 0)
				{
					this->m_changeScene = true;
				}
				else
				{
					this->m_seconds = 59;
				}
					
			}

			this->m_timeText->SetText(std::string(SceneManager::Instance()->GetText("timeText")) + std::to_string(this->m_minutes) + std::string(" : ") + std::to_string(this->m_seconds));
		}
	}
}

void GameScreen::CheckInputs(u64 kDown, u64 kHeld, u64 kUp)
{
	if (kDown & KEY_TOUCH)
	{
		u32 i;
		hidTouchRead(&this->touch, i);

		for (int y = 0; y < m_tokens.size(); y++)
		{
			for (int x = 0; x < m_tokens[y].size(); x++)
			{
				if (m_tokens[y][x]->Touched(&touch))
				{						
					if (MovePiece(x, y))
					{
						CheckPuzzle();
					}
					break;
				}			
			}
		}
	}
	
	if(kHeld & KEY_TOUCH)
	{
		u32 j;
		hidTouchRead(&touch, j);

		if (this->m_resetButton->IsPressed(&touch))
		{
			//
		}

	}
	else if (kUp & KEY_TOUCH)
	{
		if (this->m_resetButton->GetPressed())
		{
			ResetGame();
			return;
		}
	}

	if (kDown & KEY_R && kDown & KEY_L)
	{
		this->m_debugMode = !this->m_debugMode;
	}

	if (this->m_debugMode && kDown & KEY_X)
	{
		ShufflePuzzle();
		CheckPuzzle();
	}

	if (this->m_debugMode && kDown & KEY_MINUS)
	{
		m_steps = 0;
		m_minutes = 100;
	}

	if (kDown & KEY_PLUS)
	{
		m_changeScene = true;
	}
}

void GameScreen::CheckPuzzle()
{
	int counter = 0;

	for (int y = 0; y < this->m_settings->m_yTokens; y++)
	{
		for (int x = 0; x < this->m_settings->m_xTokens; x++)
		{
			if (this->m_tokens[y][x]->GetValue() == this->m_correct[y][x])
			{
				//this->m_text->SetText("Right:" + std::to_string(this->m_correct[y][x]) + "and value: " + std::to_string(m_tokens[y][x]->GetValue()));
				++counter;
			}
				
		}
	}

	//this->m_text->SetText("RIGHT:" + std::to_string(counter));

	if (counter >= this->m_settings->m_yTokens * this->m_settings->m_xTokens)
		NextScene();
}

void GameScreen::ShufflePuzzle()
{
	int m = 0;
	int n = 0;

	int value_tokenA = 0;
	int currentXA = 0;
	int currentYA = 0;

	int currentXB = 0;
	int currentYB = 0;
	int value_tokenB = 0;

	for (int y = 0; y < m_tokens.size(); y++)
	{
		for (int x = 0; x < m_tokens[y].size(); x++)
		{
			m = rand() % (this->m_settings->m_yTokens - 1);
			n = rand() % (this->m_settings->m_xTokens - 1);

			if (m != y && x != n)
			{
				currentXA = this->m_tokens[y][x]->GetCurrentFrameX();
				currentYA = this->m_tokens[y][x]->GetCurrentFrameY();
				value_tokenA = this->m_tokens[y][x]->GetValue();

				currentXB = this->m_tokens[m][n]->GetCurrentFrameX();
				currentYB = this->m_tokens[m][n]->GetCurrentFrameY();
				value_tokenB = this->m_tokens[m][n]->GetValue();

				m_tokens[m][n]->SetCurrentFrameX(currentXA);
				m_tokens[m][n]->SetCurrentFrameY(currentYA);
				m_tokens[m][n]->SetValue(value_tokenA);

				m_tokens[y][x]->SetCurrentFrameX(currentXB);
				m_tokens[y][x]->SetCurrentFrameY(currentYB);
				m_tokens[y][x]->SetValue(value_tokenB);
			}
		}
	}

}

bool GameScreen::MovePiece(int x, int y)
{
	bool change = false;
	int m = 0;
	int n = 0;

	int value_tokenA = 0;
	int currentXA = 0;
	int currentYA = 0;

	int currentXB = 0;
	int currentYB = 0;
	int value_tokenB = 0;

	if (!change && y + 1 <= this->m_settings->m_yTokens - 1)
	{
		

		m = y + 1;
		n = x;

		if (m_tokens[m][n]->GetValue() == 16) // Down
		{		
			//this->m_text->SetText("DOWN - VALUE:" + std::to_string(m_tokens[y + 1][x]->GetValue()) + " and clicked X:" + std::to_string(x) + " Y: " + std::to_string(y));
			change = true;
		}
	}
	
	if (!change && y - 1 >= 0)
	{
		m = y - 1;
		n = x;

		if (m_tokens[m][n]->GetValue() == 16) // Up
		{
			//this->m_text->SetText("UP - VALUE:" + std::to_string(m_tokens[y - 1][x]->GetValue()) + " and clicked X:" + std::to_string(x) + " Y: " + std::to_string(y));
			change = true;
		}
	}

	if (!change && x - 1 >= 0)
	{
		m = y;
		n = x - 1;

		if (m_tokens[m][n]->GetValue() == 16) // Left
		{
			//this->m_text->SetText("LEFT - VALUE:" + std::to_string(m_tokens[y][x - 1]->GetValue()) + " X:" + std::to_string(x) + " Y: " + std::to_string(y));
			change = true;
		}
	}

	if (!change && x + 1 <= this->m_settings->m_xTokens - 1)
	{
		m = y;
		n = x + 1;

		if (m_tokens[m][n]->GetValue() == 16) // Right
		{
			//this->m_text->SetText("RIGHT - VALUE:" + std::to_string(m_tokens[y][x + 1]->GetValue()) + " and clicked X:" + std::to_string(x) + " Y: " + std::to_string(y));
			change = true;
		}
	}

	if (change)
	{
		currentXA = this->m_tokens[y][x]->GetCurrentFrameX();
		currentYA = this->m_tokens[y][x]->GetCurrentFrameY();
		value_tokenA = this->m_tokens[y][x]->GetValue();

		currentXB = this->m_tokens[m][n]->GetCurrentFrameX();
		currentYB = this->m_tokens[m][n]->GetCurrentFrameY();
		value_tokenB = this->m_tokens[m][n]->GetValue();

		m_tokens[m][n]->SetCurrentFrameX(currentXA);
		m_tokens[m][n]->SetCurrentFrameY(currentYA);
		m_tokens[m][n]->SetValue(value_tokenA);

		m_tokens[y][x]->SetCurrentFrameX(currentXB);
		m_tokens[y][x]->SetCurrentFrameY(currentYB);
		m_tokens[y][x]->SetValue(value_tokenB);

		++this->m_steps;
		this->m_stepsText->SetText(std::string(SceneManager::Instance()->GetText("stepText")) + std::to_string(this->m_steps));
	}

	return change;
}

// * We go to the next scene = GameScreen
void GameScreen::NextScene()
{
	this->m_settings->m_debugMode = this->m_debugMode;
	SceneManager::Instance()->SaveData(this->m_steps);
	SceneManager::Instance()->LoadScene(SceneManager::TITLE);
}


void GameScreen::ResetGame()
{
	this->m_minutes = this->m_settings->m_maxTime;
	this->m_seconds = 59;
	this->m_steps = 0;
	this->m_stepsText->SetText(std::string(SceneManager::Instance()->GetText("stepText")) + std::to_string(this->m_steps));
	this->m_timeText->SetText(std::string(SceneManager::Instance()->GetText("timeText")) + std::to_string(this->m_minutes) + std::string(":") + std::to_string(this->m_seconds));

	ShufflePuzzle();
}