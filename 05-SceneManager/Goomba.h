#pragma once
#include "GameObject.h"

#define GOOMBA_GRAVITY 0.002f
#define GOOMBA_WALKING_SPEED 0.03f
#define GOOMBA_IS_ATTACK_SPEED_Y 0.3f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 14
#define GOOMBA_RED_WING_BBOX_HEIGHT 16
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_DIE_TIMEOUT 500

#define GOOMBA_STATE_WALKING 100
#define GOOMBA_STATE_DIE 200
#define GOOMBA_RED_WING_STATE_WALKING 201
#define GOOMBA_RED_WING_STATE_JUMP_LOW 202
#define GOOMBA_RED_WING_STATE_JUMP_HIGH 203

#define ID_ANI_GOOMBA_WALKING 30001
#define ID_ANI_GOOMBA_DIE 30002
#define ID_ANI_GOOMBA_IS_ATTACKED 30003
#define ID_ANI_GOOMBA_RED_WALK 30004
#define ID_ANI_GOOMBA_RED_WING_WALK 30006
#define ID_ANI_GOOMBA_RED_WING_JUMP 30007
#define ID_ANI_GOOMBA_RED_DIE 30005

#define LIMIT_HIGH_LOW 0.25f
#define LIMIT_HIGH_JUMP 0.4f
#define LIMIT_TIME_WING_WALKING 5000
#define LIMIT_JUMP_STACK 3

#define JUMP_LOW_SPEED 0.25f
#define JUMP_HIGH_SPEED 0.4f

#define GOOMBA_BASE 1
#define GOOMBA_RED_WING 2
class CGoomba : public CGameObject
{
protected:
	float ax;				
	float ay; 

	ULONGLONG die_start;
	ULONGLONG wing_walk_start;
	ULONGLONG jump_low_start;
	ULONGLONG jump_high_start;

	BOOLEAN isOnPlatform;
	BOOLEAN isJumpStart = false;
	BOOLEAN isOnAir = false;
	BOOLEAN isWalking = false;
	int jumpStack = 0;
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

	virtual int IsCollidable();
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public: 	
	CGoomba(float x, float y, int model);
	virtual void SetState(int state);
	int GetModel() { return model; }
};