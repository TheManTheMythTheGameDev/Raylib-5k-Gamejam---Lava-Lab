#include "GameObject2D.h"

GameObject2D::GameObject2D()
{
	tex = { 0 };
	origin = { 0.0f };
	rotation = 0.0f;
	scale = 1.0f;
	position = { 0.0f };
}

GameObject2D::GameObject2D(const char* fileName)
{
	tex = LoadTexture(fileName);
	origin = { 0.0f };
	rotation = 0.0f;
	scale = 1.0f;
	position = { 0.0f };
}

bool GameObject2D::DetectHover()
{
	Vector2 MousePosition = GetMousePosition();
	Rectangle bounding = Rectangle{ position.x - (tex.width/2) - (((tex.width / 2) * scale) / 2), position.y - (tex.height / 2) - (((tex.height / 2) * scale) / 2), tex.width + ((tex.width * scale) / 2), tex.height + ((tex.height * scale) / 2) };
	if (MousePosition.x >= bounding.x && MousePosition.x <= bounding.x + bounding.width && MousePosition.y >= bounding.y && MousePosition.y <= bounding.y + bounding.height)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GameObject2D::Render()
{
	DrawTexturePro(tex, Rectangle{ 0.0f, 0.0f, (float)tex.width, (float)tex.height }, Rectangle{ position.x - (((tex.width/2) * scale) / 2), position.y - (((tex.height/2) * scale) / 2), tex.width + ((tex.width * scale) / 2), tex.height + ((tex.height * scale) / 2) }, origin, rotation, WHITE);
}

void GameObject2D::Unload()
{
	UnloadTexture(tex);
}