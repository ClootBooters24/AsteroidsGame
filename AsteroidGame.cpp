//This is an Asteroid game replica
//Followed along on youtube to javidx9 to create this as well as add my own tweaks and changes
// olcConsoleGameEngine also created by javidx9

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
	int nScore = 0;
	bool bDead = false;

	vector<pair<float, float>> vecModelShip;
	vector<pair<float, float>> vecModelAsteroid;

protected:

	//Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
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
			float radius = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			//Breaks up a circle into degrees per segment
			float a = ((float)i / (float)verts) * 6.28318f;
			vecModelAsteroid.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
		}

		ResetGame();

		return true;
	}

	//Checking player and bullet collision to asteroids
	bool IsPointInsideCircle(float cx, float cy, float radius, float x, float y)
	{
		return sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) < radius;
	}

	void ResetGame()
	{
		vecAsteroids.clear();
		vecBullets.clear();

		vecAsteroids.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });
		vecAsteroids.push_back({ 100.0f, 20.0f, -5.0f, 3.0f, (int)16, 0.0f });

		//Initialise Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		bDead = -false;
		nScore = 0;
	}

	//Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bDead)
			ResetGame();

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

		//Check ship collision with asteroids
		for (auto& a : vecAsteroids)
			if (IsPointInsideCircle(a.x, a.y, a.nSize, player.x, player.y))
				bDead = true;

		//Fire Bullet in Direction of Player
		if (m_keys[VK_SPACE].bReleased)
			vecBullets.push_back({ player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 0, 0 });

		//Update and draw asteroids
		for (auto& a : vecAsteroids)
		{
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime;
			WrapCoordinates(a.x, a.y, a.x, a.y);

			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, a.nSize);
		}

		//Temporary vector to store new asteroids 
		//Prevents making changes to asteroids vector - will crash
		vector<sSpaceObject> newAsteroids;

		//Update and draw bullets
		for (auto& b : vecBullets)
		{
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WrapCoordinates(b.x, b.y, b.x, b.y);
			Draw(b.x, b.y);

			//Checking bullet collision with asteroids
			for (auto& a : vecAsteroids)
			{
				if (IsPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y))
				{
					//Asteroid hit
					b.x = -100;

					if (a.nSize > 4)
					{
						//Create two child asteroids
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), (int)a.nSize >> 1, 0.0f});
						newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), (int)a.nSize >> 1, 0.0f });
					}

					a.x = -100;
					nScore += 100;
				}
			}
		}

		//Append new asteroids to existing vector
		for (auto &a : newAsteroids)
			vecAsteroids.push_back(a);

		//Remove off screen bullets
		if (vecBullets.size() > 0)
		{
			auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) {return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() || o.y >= ScreenHeight() - 1); });

			if (i != vecBullets.end())
				vecBullets.erase(i);
		}

		//Remove destroyed asteroids
		if (vecAsteroids.size() > 0)
		{
			auto i = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.x < 0); });
			if (i != vecAsteroids.end())
				vecAsteroids.erase(i);
		}

		//Checking if player has cleared all asteroids
		if (vecAsteroids.empty())
		{
			nScore += 1000;

			//Add two new asteroids, but in a place where the player is not
			//Add them 90 degrees to the left and right of the player
			//Asteroids are them wrapped by the next asteroid update
			vecAsteroids.push_back({ 30.0f * sinf(player.angle - 3.10159f / 2.0f),
									 30.0f * cosf(player.angle - 3.14159f / 2.0f),
									 10.0f * sinf(player.angle),
									 10.0f * cosf(player.angle),
									 (int)16, 0.0f });
			vecAsteroids.push_back({ 30.0f * sinf(player.angle + 3.10159f / 2.0f),
									 30.0f * cosf(player.angle + 3.14159f / 2.0f),
									 10.0f * sinf(-player.angle),
									 10.0f * cosf(-player.angle),
									 (int)16, 0.0f });

		}

		//Drawing the player ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		//Drawing the score
		DrawString(2, 2, L"Score: " + to_wstring(nScore));

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
	virtual void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}
};

int main()
{
	//Use olcConsoleGameEngine derived app
	Clay_Asteroids game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	return 0;
}