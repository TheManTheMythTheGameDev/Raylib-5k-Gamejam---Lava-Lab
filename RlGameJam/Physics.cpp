#include "Physics.h"

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxFoundation* gFoundation;
static PxPhysics* gPhysics;
static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
static PxIDENTITY gIdentity;
static PxCooking* gCooking;
static PxMaterial* gMaterial;

static PxScene* gScene = NULL;

float accumulator;
float step = 1.0f / 60.0f;

bool InitPhysX()
{

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!gFoundation)
		printf("PxCreateFoundation failed!");

	bool recordMemoryAllocations = true;

	PxTolerancesScale toleranceScale;
	toleranceScale.length = 1;
	toleranceScale.speed = 9;

	gPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *gFoundation, toleranceScale, recordMemoryAllocations);

	PxRegisterArticulations(*gPhysics);
	PxRegisterHeightFields(*gPhysics);
	if (!gPhysics)
	{
		printf("PxCreatePhysics failed!");
		return 1;
	}
	if (!PxInitExtensions(*gPhysics, NULL))
	{
		printf("PxInitExtensions failed!");
		return 1;
	}

	PxTolerancesScale scale = gPhysics->getTolerancesScale();
	PxCookingParams params = PxCookingParams(scale);

	bool value = toleranceScale.isValid();
	if (value != true) {
		printf("ToleranceScale invalid");
		return 1;
	}

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, gPhysics->getFoundation(), params);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!mCpuDispatcher)
			printf("PxDefaultCpuDispatcherCreate failed!");
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	sceneDesc.filterShader = gDefaultFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	if (!gScene)
	{
		printf("createScene failed!");
		return 1;
	}

	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);

	gIdentity = PxIdentity();
	return 0;
}

bool ClosePhysX()
{
	PxCloseExtensions();
	gCooking->release();
	gMaterial->release();
	gScene->release();
	gPhysics->release();
	gFoundation->release();
	return 0;
}

void StepPhysX(float dt)
{
	accumulator += dt;
	if (!(accumulator < step))
	{
		accumulator -= step;

		//if the previous call to fetch result has been completed
		gScene->simulate(step);

		gScene->fetchResults(true);
	}
}

PxRigidDynamic* CreateBox(Vector3 position, Vector3 scale, Vector3 velocity, float density)
{
	PxReal pDensity = density;
	PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat(gIdentity));
	PxVec3 dimensions(scale.x, scale.y, scale.z);
	PxBoxGeometry geometry(dimensions);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysics, transform, geometry, *gMaterial, pDensity);
	if (!actor)
		printf("create actor failed!");
	actor->setAngularDamping(0.75);
	// actor->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	gScene->addActor(*actor);
	return actor;
}

PxRigidStatic* CreateStaticBox(Vector3 position, Vector3 scale, float density)
{
	PxReal pDensity = density;
	PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat(gIdentity));
	PxVec3 dimensions(scale.x, scale.y, scale.z);
	PxBoxGeometry geometry(dimensions);

	PxRigidStatic* actor = PxCreateStatic(*gPhysics, transform, geometry, *gMaterial);
	if (!actor)
		printf("create actor failed!");
	gScene->addActor(*actor);
	return actor;
}

PxRigidDynamic* CreateSphere(Vector3 position, float radius, Vector3 velocity, float density)
{
	PxReal pDensity = density;
	PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat(gIdentity));
	PxReal r = radius;
	PxSphereGeometry geometry(r);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysics, transform, geometry, *gMaterial, pDensity);
	if (!actor)
		printf("create actor failed!");
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	PxVec3 vel = PxVec3(velocity.x, velocity.y, velocity.z);
	gScene->addActor(*actor);

	return actor;
}

void CreateSnowball(Vector3 position, PxVec3 velocity, float radius, int spheres, Vector3 impactPoint, float density)
{
	PxReal pDensity = density;
	PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat(gIdentity));
	PxReal r = radius / ((float)spheres);
	PxBoxGeometry geom(r, r, r);

	int hSph = spheres / 2;
	for (int x = -hSph; x < hSph; x++)
	{
		for (int y = -hSph; y < hSph; y++)
		{
			for (int z = -hSph; z < hSph; z++)
			{
				if (Vector3Distance(Vector3{ 0, 0, 0 }, Vector3{ x * r, y * r, z * r }) < radius / 2)
				{
					PxVec3 voxelPosition = PxVec3(((x * 2.0f) * r) + position.x, ((y * 2.0f) * r) + position.y, ((z * 2.0f) * r) + position.z);
					PxRigidDynamic* actor = PxCreateDynamic(*gPhysics, PxTransform(voxelPosition, PxQuat(gIdentity)), geom, *gMaterial, pDensity);

					actor->setAngularDamping(0.75);
					actor->setLinearVelocity(velocity + PxVec3((voxelPosition.x - impactPoint.x) * 1.0f, (voxelPosition.y - impactPoint.y) * 1.0f, (voxelPosition.z - impactPoint.z) * 1.0f));
					actor->setName("balls");
					actor->setLinearDamping(10.0);
					gScene->addActor(*actor);
				}
			}
		}
	}
}

PxRigidDynamic* CreateCapsule(Vector3 position, float height, float radius, Vector3 velocity, float density)
{
	PxReal pDensity = density;
	PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
	PxReal pRadius = radius;
	PxReal pHeight = height;
	PxCapsuleGeometry geometry(pRadius, pHeight / 2);

	PxRigidDynamic* actor = PxCreateDynamic(*gPhysics, transform, geometry, *gMaterial, pDensity);
	actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	if (!actor)
		printf("create actor failed!");
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	gScene->addActor(*actor);
	return actor;
}

PxRigidStatic* CreatePlane(Vector3 position, PxQuat rotation)
{
	PxReal d = 0.0f;
	PxTransform pose = PxTransform(PxVec3(position.x, position.y, position.z), rotation);

	PxRigidStatic* plane = gPhysics->createRigidStatic(pose);
	if (!plane)
		printf("create plane failed!");
	PxShape* shape = gPhysics->createShape(PxPlaneGeometry(), *gMaterial);
	plane->attachShape(*shape);
	if (!shape)
		printf("create shape failed");
	gScene->addActor(*plane);
	return plane;
}

PxRigidDynamic* CreateMesh(Vector3 position, Model& model, unsigned int meshID, float scale, float density)
{
	PxReal pDensity = density;
	PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat(gIdentity));

	PxTriangleMeshDesc meshDesc;
	PxVec3* vertData = (PxVec3*)model.meshes[meshID].vertices;
	PxU16* triData = model.meshes[meshID].indices;

	meshDesc.setToDefault();
	meshDesc.flags |= PxMeshFlag::e16_BIT_INDICES;
	meshDesc.points.count = static_cast<PxU32>(model.meshes[meshID].vertexCount);
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertData;

	meshDesc.triangles.count = static_cast<PxU32>(model.meshes[meshID].triangleCount);
	meshDesc.triangles.stride = sizeof(PxU16) * 3;
	meshDesc.triangles.data = triData;

#ifdef _DEBUG
	// mesh should be validated before cooked without the mesh cleaning
	bool res = gCooking->validateTriangleMesh(meshDesc);
	PX_ASSERT(res);
#endif

	PxTriangleMesh* triMesh = NULL;
	triMesh = gCooking->createTriangleMesh(meshDesc,
		gPhysics->getPhysicsInsertionCallback());

	PxTriangleMeshGeometry geom = PxTriangleMeshGeometry(triMesh);

	PxShape* shape = gPhysics->createShape(geom, *gMaterial, true, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE);
	shape->setName("shape");
	PxFilterData filterData;
	filterData.word0 = GROUND;
	shape->setQueryFilterData(filterData);
	PxRigidDynamic* body = gPhysics->createRigidDynamic(PxTransform(PxVec3(position.x, position.y, position.z), PxQuat(gIdentity)));
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	body->attachShape(*shape);
	PxReal pScale = scale;
	PxScaleRigidActor(*body, pScale);
	body->setAngularDamping(0.75);
	// body->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
	body->userData = (void*)&model;
	gScene->addActor(*body);
	return body;
}

void DrawBox(PxShape* pShape, PxRigidActor* actor, Model& cube, Vector3 scale)
{
	PxTransform pT = PxShapeExt::getGlobalPose(*pShape, *actor);
	cube.transform = MatrixMultiply(MatrixScale(scale.x, scale.y, scale.z), MatrixMultiply(QuaternionToMatrix(Quaternion{ pT.q.x, pT.q.y, pT.q.z, pT.q.w }), MatrixTranslate(pT.p.x, pT.p.y, pT.p.z)));
	DrawModel(cube, Vector3Zero(), 1.0f, WHITE);
}

PxRaycastBuffer PhysXRaycast(PxVec3 origin, PxVec3 unitDir, PxReal maxDistance)
{
	PxRaycastBuffer hit;

	PxQueryFilterData filterData = PxQueryFilterData();
	filterData.data.word0 = GROUND;
	gScene->raycast(origin, unitDir, maxDistance, hit, PxHitFlag::eDEFAULT | PxHitFlag::ePOSITION, filterData);
	return hit;
}

PxScene* GetPhysXScene()
{
	return gScene;
}