#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

//Downloaded olcConsoleGameEngine from GitHub to use the console as a game engine
//olcConsoleGameEngine as header file
#include "olcConsoleGameEngine.h"

class Clay_Asteroids : public olcConsoleGameEngine
{
public:
	Clay_Asteroids()
	{
		m_sAppName = L"Asteroids";
	}

private:
	struct sSpaceObject
	{
		float x;
		float y;
		float dx;
		float dy;
		int nSize;
	};

	vector<sSpaceObject> vecAsteroids;

protected:

	//Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		vecAsteroids.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16 });
		
		return true;
	}

	//Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		//Update and draw asteroids
		for (auto& a : vecAsteroids)
		{
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;

			for (int x = 0; x < a.nSize; x++)
				for (int y = 0; y < a.nSize; y++)
					Draw(a.x + x, a.y + y, PIXEL_QUARTER, FG_RED);
		}

		return true;
	}
};

int main()
{
	//Use oldConsoleGameEngine derived app
	Clay_Asteroids game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	return 0;
}