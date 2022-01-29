#pragma once
#include <raylib/raylib.h>
#include "Physics.h"

class GameObject
{
public:
	GameObject();
	GameObject(const char* _model_path);
	void Unload();
	void Update();
	void Draw();
	Matrix transform;
	Model model;
	PxRigidDynamic* dyn;
	PxRigidStatic* sta;
	bool isDyn;
	bool BindToPxBody;
};