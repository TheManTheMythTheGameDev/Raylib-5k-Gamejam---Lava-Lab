#pragma once
#include "GameObject.h"

struct Level
{
	GameObject obj;
	BoundingBox done;
	Level* next;
	bool CheckDone(Vector3 playerPos);
	void SwitchTo();
	void SwitchOut();
};