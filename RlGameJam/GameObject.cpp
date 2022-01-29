#include "GameObject.h"

GameObject::GameObject()
{
	Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
	model = LoadModelFromMesh(cube);
	dyn = CreateBox(Vector3Zero(), Vector3One(), Vector3Zero(), 10.0f);
	transform = MatrixIdentity();
	isDyn = true;
	BindToPxBody = false;
}

GameObject::GameObject(const char* _model_path)
{
	model = LoadModel(_model_path);
	dyn = CreateBox(Vector3Zero(), Vector3One(), Vector3Zero(), 10.0f);
	transform = MatrixIdentity();
	isDyn = true;
	BindToPxBody = false;
}

void GameObject::Unload()
{
	UnloadModel(model);
	if (dyn) dyn->release();
	if (sta) sta->release();
}

void GameObject::Draw()
{
	DrawModel(model, Vector3Zero(), 1.0f, WHITE);
}

void GameObject::Update()
{
	if (BindToPxBody)
	{
		PxTransform pT;
		if (isDyn)
		{
			pT = dyn->getGlobalPose();
		}
		else
		{
			pT = sta->getGlobalPose();
		}
		transform = MatrixMultiply(QuaternionToMatrix(Quaternion{ pT.q.x, pT.q.y, pT.q.z, pT.q.w }), MatrixTranslate(pT.p.x, pT.p.y, pT.p.z));
	}

	model.transform = transform;
}