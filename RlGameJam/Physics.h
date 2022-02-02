#pragma once
#include <px/PxPhysicsAPI.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>

enum ActiveGroup
{
	GROUND = (1 << 0),
	PLAYER = (1 << 1),
	BOUNCE_PAD = ( 1 << 2),
	GRAPPLE_PAD = ( 1 << 3)
};

using namespace physx;

bool InitPhysX();

bool ClosePhysX();

void StepPhysX(float dt);

PxRigidDynamic* CreateSphere(Vector3 position, float radius, Vector3 velocity, float density);
PxRigidDynamic* CreateBox(Vector3 position, Vector3 scale, Vector3 velocity, float density);
PxRigidStatic* CreateStaticBox(Vector3 position, Vector3 scale, float density);
PxRigidDynamic* CreateCapsule(Vector3 position, float height, float radius, Vector3 velocity, float density);
PxRigidDynamic* CreateMesh(Vector3 position, Model& model, unsigned int meshID, float scale, float density, ActiveGroup filterGroup = GROUND);
PxRigidStatic* CreatePlane(Vector3 position, PxQuat rotation);

PxRaycastBuffer PhysXRaycast(PxVec3 origin, PxVec3 unitDir, PxReal maxDistance, ActiveGroup filter = GROUND);
PxScene* GetPhysXScene();