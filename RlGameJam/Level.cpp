#include "Level.h"

bool Level::CheckDone(Vector3 playerPos)
{
	return (playerPos.x > done.min.x && playerPos.x < done.max.x&& playerPos.y > done.min.y && playerPos.y < done.max.y&& playerPos.z > done.min.z && playerPos.z < done.max.z);
}

void Level::Update()
{
	for (int i = 0; i < layers.size(); i++)
	{
		layers[i]->Update();
	}
}

void Level::Draw()
{
	for (int i = 0; i < layers.size(); i++) layers[i]->Draw();
}

void Level::SwitchTo()
{
	for (int i = 0; i < layers.size(); i++) layers[i]->dyn->setGlobalPose(PxTransform(PxVec3(0.0f)));
	isActive = true;
}

void Level::SwitchOut()
{
	for (int i = 0; i < layers.size(); i++) layers[i]->dyn->setGlobalPose(PxTransform(PxVec3(-100, 0, 0)));
	if (next) next->SwitchTo();
	isActive = false;
}

void Level::Unload()
{
	for (int i = 0; i < layers.size(); i++) layers[i]->Unload();
}