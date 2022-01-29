#pragma once
#include <raylib/raylib.h>
class GameObject2D
{
public:
	GameObject2D();
	GameObject2D(const char* fileName);
	Texture2D tex;
	Vector2 origin;
	float rotation;
	float scale;
	Vector2 position;
	bool DetectHover();
	void Render();
	void Unload();
};