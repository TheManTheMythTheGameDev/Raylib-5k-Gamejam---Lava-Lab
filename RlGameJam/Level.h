#pragma once
#include "GameObject.h"

#include <vector>

using namespace std;

struct Level
{
	vector<GameObject*> layers;
	BoundingBox done;
	bool isActive = false;
	vector<int> availableTools = { 0 };
	bool isGrapplingGunLimited = false;
	Level* next;
	bool CheckDone(Vector3 playerPos);
	void Update();
	void Draw();
	void SwitchTo();
	void SwitchOut();
	void Unload();
};