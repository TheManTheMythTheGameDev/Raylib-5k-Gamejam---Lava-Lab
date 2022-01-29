#include <raylib/raylib.h>
#include <raylib/rlgl.h>
#include "GameObject.h"
#include "GameObject2D.h"
#include "rlFPCamera.h"
#include <raylib/extras/easings.h>
#include "rlpbr.h"
#include "Level.h"

#define PI 3.141592653589793

void Restart(Vector3& lp, GameObject& pl);

enum GameState
{
	INTRO,
	TITLE,
	GAMEPLAY,
	END
};

enum Tools
{
	NONE,
	GRAPPLING_GUN,
	KNOCKBACK_GUN,
	EXPLOSION_GUN
};

int main(void)
{
	const int screenWidth = 800;
	const int screenHeight = 450;
	SetTraceLogLevel(LOG_NONE);
	InitWindow(screenWidth, screenHeight, "Lava Lab");

	InitPhysX();

	InitPBR();

	InitAudioDevice();
	SetMasterVolume(100.0f);

	SetTargetFPS(60);

	// ChangeDirectory(GetApplicationDirectory());

	rlFPCamera cam;

	GameState currentState = INTRO;

	AddLight(Light{ Vector3{ -15.0f, 10, 12.0f }, Vector3{ 0, 0, 0 }, WHITE, 20.0f, POINT, true });
	AddLight(Light{ Vector3{ -15.0f, 10, 0.0f }, Vector3{ 0, 0, 0 }, WHITE, 20.0f, POINT, true });
	AddLight(Light{ Vector3{ -5.0f, 10, 12.0f }, Vector3{ 0, 0, 0 }, WHITE, 20.0f, POINT, true });
	AddLight(Light{ Vector3{ -5.0f, 10, 0.0f }, Vector3{ 0, 0, 0 }, WHITE, 20.0f, POINT, true });
	AddLight(Light{ Vector3{ -40.0f, 10, 12.0f }, Vector3{ 0, 0, 0 }, WHITE, 20.0f, POINT, true });
	AddLight(Light{ Vector3{ -40.0f, 10, 0.0f }, Vector3{ 0, 0, 0 }, WHITE, 20.0f, POINT, true });
	AddLight(Light{ Vector3{ 0, 5, 0 }, Vector3{ 0, 0, 0 }, RED, 30.0f, POINT, true });
	void* end = AddLight(Light{ Vector3{ 0, 5, 12.0f }, Vector3{ 0, 0, 12.0f }, BLUE, 30.0f, POINT, true });

	Texture2D texture = LoadTexture("introgif.png");        // Texture loading

	Vector2 position = { 0.0f, 0.0f };
	Rectangle frameRec = { 0.0f, 0.0f, (float)texture.width / 34, (float)texture.height };
	int currentFrame = 0;

	int framesCounter = 0;
	int framesSpeed = 8;            // Number of spritesheet frames shown by second

	int framesAnimCounter = 0;

	int currentTime = 0;
	int duration = 100;
	float startScale = 0.0f;
	float finalScale = 50.0f;
	float currentScale = startScale;

	GameObject2D nameTexture = GameObject2D("LavaLab.png");
	nameTexture.origin = Vector2{ (float)nameTexture.tex.width / 2, (float)nameTexture.tex.height/2 };
	nameTexture.position = Vector2{ (float)(GetScreenWidth()/2) - (nameTexture.tex.width / 2), 100 };
	nameTexture.scale = 10.0f;

	GameObject2D playButton = GameObject2D("LLPlay.png");
	playButton.origin = Vector2{ (float)playButton.tex.width / 2, (float)playButton.tex.height / 2 };
	playButton.position = Vector2{ (float)(GetScreenWidth() / 2) - (nameTexture.tex.width / 2), 350 };
	playButton.scale = 0.3f;

	int nameFrames = 0;
	int playFrames = 0;

	GameObject player = GameObject();
	player.dyn->release();
	player.dyn = CreateCapsule(Vector3{ 0.0f, 8.0f, 0.0f }, 2.0f, 1.0f, Vector3Zero(), 10.0f);

	GameObject level1 = GameObject("lvl1.obj");
	level1.dyn->release();
	level1.dyn = CreateMesh(Vector3Zero(), level1.model, 0, 1.0f, 10.0f);
	level1.BindToPxBody = true;

	GameObject level2 = GameObject("lvl2.obj");
	level2.dyn->release();
	level2.dyn = CreateMesh(Vector3{ -100, 0, 0 }, level2.model, 0, 1.0f, 10.0f);
	level2.BindToPxBody = true;

	GameObject level3 = GameObject("lvl3.obj");
	level3.dyn->release();
	level3.dyn = CreateMesh(Vector3{ -100, 0, 0 }, level3.model, 0, 1.0f, 10.0f);
	level3.BindToPxBody = true;

	GameObject level4 = GameObject("lvl4.obj");
	level4.dyn->release();
	level4.dyn = CreateMesh(Vector3{ -100, 0, 0 }, level4.model, 0, 1.0f, 10.0f);
	level4.BindToPxBody = true;

	GameObject level5 = GameObject("lvl5.obj");
	level5.dyn->release();
	level5.dyn = CreateMesh(Vector3{ -100, 0, 0 }, level5.model, 0, 1.0f, 10.0f);
	level5.BindToPxBody = true;

	GameObject level6 = GameObject("lvl6.obj");
	level6.dyn->release();
	level6.dyn = CreateMesh(Vector3{ -100, 0, 0 }, level6.model, 0, 1.0f, 10.0f);
	level6.BindToPxBody = true;

	level1.model.materials[0] = LoadPBRMaterial(0, 0, 0, 0, 0, TEXTURE_FILTER_POINT);
	level2.model.materials[0] = LoadPBRMaterial(0, 0, 0, 0, 0, TEXTURE_FILTER_POINT);
	level3.model.materials[0] = LoadPBRMaterial(0, 0, 0, 0, 0, TEXTURE_FILTER_POINT);
	level4.model.materials[0] = LoadPBRMaterial(0, 0, 0, 0, 0, TEXTURE_FILTER_POINT);
	level5.model.materials[0] = LoadPBRMaterial(0, 0, 0, 0, 0, TEXTURE_FILTER_POINT);
	level6.model.materials[0] = LoadPBRMaterial(0, 0, 0, 0, 0, TEXTURE_FILTER_POINT);

	Level* lvl6 = new Level{ level6, BoundingBox{ Vector3{ 0, 0, 8.0f }, Vector3{ 5, 20, 16.0f } }, NULL };
	Level* lvl5 = new Level{ level5, BoundingBox{ Vector3{ 0, 0, 30.602f }, Vector3{ 5, 20, 38.602f } }, lvl6 };
	Level* lvl4 = new Level{ level4, BoundingBox{ Vector3{ 0, 0, 8.0f }, Vector3{ 5, 20, 16.0f } }, lvl5 };
	Level* lvl3 = new Level{ level3, BoundingBox{ Vector3{ 0, 0, 8.0f }, Vector3{ 5, 20, 16.0f } }, lvl4 };
	Level* lvl2 = new Level{ level2, BoundingBox{ Vector3{ 0, 0, 8.0f }, Vector3{ 5, 20, 16.0f } }, lvl3 };
	Level* allLevels = new Level{ level1, BoundingBox{ Vector3{ 0, 0, 8.0f }, Vector3{ 5, 20, 16.0f } }, lvl2 };
	allLevels->next = lvl2;

	Vector3 lavaPosition = { 0.0f, 0.0f, -5.0f };
	Model lava = LoadModel("lava.obj");
	lava.materials[0] = LoadPBRMaterial("LavaAlbedo.png", 0, 0, 0, 0, TEXTURE_FILTER_POINT);

	Tools currentTool = NONE;

	Model grapplinggun = LoadModel("grapplinggun.obj");
	grapplinggun.materials[0] = LoadPBRMaterial(0, 0, 0, 0, 0, TEXTURE_FILTER_POINT);

	Vector3 grapplePoint = Vector3{ 0.0f, 0.0f, 0.0f };
	float maxGrappleDistance = 100.0f;
	bool isGrappling = false;

	Texture2D grtex = LoadTexture("1ToUse.png");

	int levnb = 1;

	int timeTaken = 0;

	const char* text = TextFormat("YOUR TIME: %i SECONDS", timeTaken);

	Sound whoosh = LoadSound("wind_trim.mp3");
	SetSoundVolume(whoosh, 20.0f);
	Sound jump = LoadSound("Jump.mp3");
	SetSoundVolume(jump, 20.0f);

	lava.transform = MatrixMultiply(lava.transform, MatrixScale(10.0f, 10.0f, 1.0f));

	SetExitKey(KEY_NULL);

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();


		if (currentState == INTRO)
		{
			framesCounter++;

			if (framesCounter >= (GetFPS() / framesSpeed))
			{
				framesCounter = 0;
				currentFrame++;

				if (currentFrame > 33)
				{
					currentFrame = 33;
					currentState = TITLE;
				}

				frameRec.x = (float)currentFrame * (float)texture.width / 34;
			}
		}

		BeginDrawing();
		{
			ClearBackground(BLACK);

			if (currentState == INTRO)
			{
				DrawTextureRec(texture, frameRec, position, WHITE);
			}

			else if (currentState == TITLE)
			{
				if (currentScale < finalScale)
				{
					currentScale = EaseSineOut(currentTime, startScale, finalScale - startScale, duration);
					currentTime++;
				}
				DrawTexturePro(texture, frameRec, Rectangle{ currentScale, currentScale, GetScreenWidth() - (currentScale * 2), GetScreenHeight() - (currentScale * 2) }, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
				if (nameFrames <= 500)
				{
					nameFrames++;
					nameTexture.scale = EaseElasticOut(nameFrames, 0.0f, 10.0f, 200);
				}

				if (playButton.DetectHover() && playButton.scale < 0.5f)
				{
					playButton.scale += 0.01f;
				}
				else if (!playButton.DetectHover() && playButton.scale > 0.3f)
				{
					playButton.scale -= 0.01f;
				}
				if (playButton.DetectHover() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					currentState = GAMEPLAY;
					cam.Setup(45.0f, Vector3{ 0.0f, 0.0f, 0.0f });
				}

				nameTexture.Render();
				playButton.Render();
			}


			else if (currentState == GAMEPLAY)
			{
				cam.Update();
				StepPhysX(dt);

				UpdatePBR(cam.GetCamera());


				player.Update();

				if (isGrappling)
				{
					Vector3 dir = Vector3Subtract(grapplePoint, cam.GetCameraPosition());
					PxVec3 pxDir = PxVec3(dir.x, dir.y, dir.z);
					player.dyn->addForce(pxDir * 500);
				}

				for (int i = 0; i < 10; i++)
				{
					int key = 48 + i;
					if (IsKeyPressed(key))
					{
						currentTool = static_cast<Tools>(i);
					}
				}

				lavaPosition.z += 1 * dt;
				if (lavaPosition.z > player.dyn->getGlobalPose().p.z)
				{
					Restart(lavaPosition, player);
				}
				if (player.dyn->getGlobalPose().p.y < -5.0f)
				{
					Restart(lavaPosition, player);
				}
				
				Level* current = allLevels;
				while (current)
				{
					current->obj.Update();
					if (current->CheckDone(cam.GetCameraPosition()) && current->obj.dyn->getGlobalPose().p.x > -100)
					{
						Restart(lavaPosition, player);
						current->SwitchOut();
						levnb++;
						if (current->next)
						{
							BoundingBox done = current->next->done;
							Vector3 pos = Vector3Scale(Vector3Add(done.min, done.max), 0.5);
							SetLight(end, Light{ Vector3{ pos.x, pos.y - 3.0f, pos.z }, Vector3{ 0, 0, 12.0f }, BLUE, 30.0f, POINT, true });
						}
						if (levnb > 6)
						{
							currentState = END;
							timeTaken = (int)GetTime();
							text = TextFormat("YOUR TIME: %i SECONDS", timeTaken);
							cam.HideCursor = false;
							cam.UseMouseX = false;
							cam.UseMouseY = false;
							cam.Update();
						}
						break;
					}
					current = current->next;
				}

				Vector3 camForward = cam.GetFowardGroundVector();
				Vector3 camRight = Vector3CrossProduct(Vector3{ 0.0f, 1.0f, 0.0f }, camForward);
				PxVec3 pCamForward = PxVec3(camForward.x, camForward.y, camForward.z);
				PxVec3 pCamRight = PxVec3(camRight.x, camRight.y, camRight.z);

				if (IsKeyDown(KEY_W))
				{
					player.dyn->addForce(pCamForward * 1000);
				}
				if (IsKeyDown(KEY_S))
				{
					player.dyn->addForce(-pCamForward * 1000);
				}
				if (IsKeyDown(KEY_D))
				{
					player.dyn->addForce(-pCamRight * 1000);
				}
				if (IsKeyDown(KEY_A))
				{
					player.dyn->addForce(pCamRight * 1000);
				}
				if (IsKeyPressed(KEY_SPACE))
				{
					Vector3 rlcp = cam.GetCameraPosition();
					PxVec3 pxcp = PxVec3(rlcp.x, rlcp.y, rlcp.z);
					PxVec3 dir = PxVec3(0.0f, -1.0f, 0.0f);
					PxReal maxDist = 2.0f;
					if (PhysXRaycast(pxcp, dir, maxDist).hasAnyHits())
					{
						player.dyn->addForce(PxVec3(0.0f, 50000.0f, 0.0f));
						PlaySound(jump);
					}
				}

				if (IsKeyPressed(KEY_ESCAPE))
				{
					if (IsCursorHidden())
					{
						cam.HideCursor = false;
						cam.UseMouseX = false;
						cam.UseMouseY = false;
					}
					else
					{
						cam.HideCursor = true;
						cam.UseMouseX = true;
						cam.UseMouseY = true;
					}
				}

				PxTransform plT = player.dyn->getGlobalPose();
				cam.SetCameraPosition(Vector3{ plT.p.x, plT.p.y, plT.p.z });

				Vector3 camPos = Vector3Add(cam.GetCameraPosition(), Vector3Add(Vector3Scale(cam.GetForwardVector(), 2.0f), Vector3Scale(camRight, -1.0f)));

				cam.BeginMode3D();
				{
					Level* cld = allLevels;
					while (cld)
					{
						cld->obj.Draw();
						cld = cld->next;
					}
					DrawModel(lava, lavaPosition, 1.0f, WHITE);
					if (currentTool == GRAPPLING_GUN)
					{
						Vector3 D = Vector3Normalize(cam.GetForwardVector());
						if (isGrappling) D = Vector3Normalize(Vector3Subtract(grapplePoint, camPos));
						Vector3 U = { 0.0f, 1.0f, 0.0f };
						Vector3 R = Vector3CrossProduct(U, D);
						float angle_H = atan2(D.y, D.x);
						float angle_P = asin(D.z);
						Vector3 W0 = Vector3{ -D.y, D.x, 0 };
						Vector3 U0 = Vector3CrossProduct(W0, D);
						float angle_B = atan2(Vector3DotProduct(W0, U), Vector3DotProduct(U0, U) / Vector3Length(W0) * Vector3Length(U0));
						Vector3 grappleAngle = Vector3{ angle_B, angle_P, -angle_H };
						grapplinggun.transform = MatrixMultiply(MatrixIdentity(), MatrixMultiply(MatrixRotateXYZ(grappleAngle), MatrixTranslate(camPos.x, camPos.y, camPos.z)));
						DrawModel(grapplinggun, Vector3Zero(), 1.0f, WHITE);

						if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
						{
							PxRaycastBuffer grappleRay = PhysXRaycast(PxVec3(camPos.x, camPos.y, camPos.z), PxVec3(D.x, D.y, D.z), maxGrappleDistance);
							float grapCeil = 10.0f;
							if (levnb == 4 || levnb == 5 || levnb == 6) grapCeil = 44.0f;
							if (!isGrappling && grappleRay.hasAnyHits() && grappleRay.getAnyHit(0).position.y < grapCeil)
							{
								PxVec3 gp = grappleRay.getAnyHit(0).position;
								grapplePoint = Vector3{ gp.x, gp.y, gp.z };
								isGrappling = true;
								PlaySound(whoosh);
							}
							if (isGrappling)
							{
								rlDrawRenderBatchActive();
								rlSetLineWidth(4000.0f);
								DrawLine3D(camPos, grapplePoint, BLACK);
								rlDrawRenderBatchActive();
								rlSetLineWidth(1.0f);
							}
						}
						else
						{
							isGrappling = false;
						}
					}
					if (levnb == 3)
					{
						DrawBillboard(cam.GetCamera(), grtex, Vector3{ -3.0f, 5.0f, 3.0f }, 8.0f, WHITE);
					}
				}
				cam.EndMode3D();

				DrawLine((GetScreenWidth() / 2) - 20.0f, (GetScreenHeight() / 2), (GetScreenWidth() / 2) + 20.0f, (GetScreenHeight() / 2), BLACK);
				DrawLine((GetScreenWidth() / 2), (GetScreenHeight() / 2) - 20.0f, (GetScreenWidth() / 2), (GetScreenHeight() / 2) + 20.0f, BLACK);
			}
			if (currentState == END)
			{
				int fontSize = 80;
				DrawText("YOU WIN", (GetScreenWidth() / 2) - (MeasureText("YOU WIN", fontSize) / 2), (GetScreenHeight() / 2) - (fontSize/2), fontSize, WHITE);
				fontSize = 40;
				DrawText(text, (GetScreenWidth() / 2) - (MeasureText(text, fontSize) / 2), (GetScreenHeight() / 2) - (fontSize / 2) + 100, fontSize, WHITE);
			}
		}
		EndDrawing();
	}

	UnloadTexture(texture);
	nameTexture.Unload();

	UnloadTexture(grtex);

	player.Unload();
	level1.Unload();
	level2.Unload();
	level3.Unload();
	level4.Unload();
	level5.Unload();
	level6.Unload();

	UnloadSound(whoosh);
	UnloadSound(jump);

	UnloadModel(grapplinggun);

	UnloadModel(lava);

	ClosePhysX();

	CloseAudioDevice();

	ClosePBR();

	CloseWindow();

	return 0;
}

void Restart(Vector3& lp, GameObject& pl)
{
	lp = Vector3{ 0.0f, 0.0f, -5.0f };
	pl.dyn->setLinearVelocity(PxVec3(0.0f));
	pl.dyn->setGlobalPose(PxTransform(PxVec3(0.0f, 8.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
}