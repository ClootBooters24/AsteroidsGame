//This is an Asteroid game replica
//Followed along on youtube to javidx9 to create this as well as add my own tweaks and changes
// olcConsoleGameEngine also created by javidx9 but recreated by me

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

//Downloaded olcConsoleGameEngine from GitHub to use the console as a game engine
//olcConsoleGameEngine as header file
#include "olcConsoleGameEngineGL.h"

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
		float angle;
	};

	vector<sSpaceObject> vecAsteroids;
	vector<sSpaceObject> vecBullets;
	sSpaceObject player;

	vector<pair<float, float>> vecModelShip;
	vector<pair<float, float>> vecModelAsteroid;

protected:

	//Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		vecAsteroids.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });
		
		//Initialise Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		//Simple Isoceles Triangle for Player Ship
		vecModelShip =
		{
			{ 0.0f, -5.0f },
			{ -2.5f, +2.5f },
			{ +2.5f, +2.5f }
		};

		//Creating the Asteroids
		int verts = 20;
		for (int i = 0; i < verts; i++)
		{
			float radius = 1.0f;
			//Breaks up a circle into degrees per segment
			float a = ((float)i / (float)verts) * 6.28318f;
			vecModelAsteroid.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
		}

		return true;
	}

	//Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		//Steer
		if (m_keys[VK_LEFT].bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (m_keys[VK_RIGHT].bHeld)
			player.angle += 5.0f * fElapsedTime;

		//Thrust
		if (m_keys[VK_UP].bHeld)
		{
			//Acceleration changes velocity with respect to time
			player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		//Velocity changes position with respect to time
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;

		//Keeps player in gamespace
		WrapCoordinates(player.x, player.y, player.x, player.y);

		//Fire Bullet in Direction of Player
		if (m_keys[VK_SPACE].bReleased)
			vecBullets.push_back({ player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 0, 0 });

		//Update and draw asteroids
		for (auto& a : vecAsteroids)
		{
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			WrapCoordinates(a.x, a.y, a.x, a.y);

			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, a.nSize);
		}

		//Update and draw bullets
		for (auto& b : vecBullets)
		{
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WrapCoordinates(b.x, b.y, b.x, b.y);
			
			Draw(b.x, b.y);
		}

		//Drawing the player ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		return true;
	}

	void DrawWireFrameModel(const vector<pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
	{
		//pair.first = x coordinate
		//pair.second = y coordinate

		//Create translated model vector of coordinate pairs
		vector<pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		//Rotation
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate Rotation
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, col);
		}
	}

	void WrapCoordinates(float ix, float iy, float& ox, float& oy)
	{
		ox = ix;
		oy = iy;

		if (ix < 0.0f) ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth()) ox = ix - (float)ScreenWidth();
		if (iy < 0.0f) oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
	}

	//Game will not wrap correctly
	//This function will override ConsoleGameEngine and allow the game to wrap around correctly
	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x00F0)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}
};

int main()
{
	//Use oldConsoleGameEngine derived app
	Clay_Asteroids game;
	game.ConstructConsole(100, 80, 8, 8);
	game.Start();
	return 0;
}