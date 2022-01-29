#include "Level.h"

bool Level::CheckDone(Vector3 playerPos)
{
	return (playerPos.x > done.min.x && playerPos.x < done.max.x&& playerPos.y > done.min.y && playerPos.y < done.max.y&& playerPos.z > done.min.z && playerPos.z < done.max.z);
}

void Level::SwitchTo()
{
	obj.dyn->setGlobalPose(PxTransform(PxVec3(0.0f)));
}

void Level::SwitchOut()
{
	obj.dyn->setGlobalPose(PxTransform(PxVec3(-100, 0, 0)));
	if (next) next->SwitchTo();
}