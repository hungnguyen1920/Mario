#pragma once
#include "GameObject.h"

#define FPP_BBOX_WIDTH 16
#define FPP_BIG_BBOX_HEIGHT 33
#define FPP_SMALL_BBOX_HEIGHT 25

// ID ANI FPP

// BIG
#define ID_ANI_FPP_LEFT_TOP 50002
#define ID_ANI_FPP_LEFT_BOTTOM 50003
#define ID_ANI_FPP_LEFT_TOP_SHOOT 50004
#define ID_ANI_FPP_LEFT_BOTTOM_SHOOT 50005
#define ID_ANI_FPP_RIGHT_TOP 50006
#define ID_ANI_FPP_RIGHT_BOTTOM 50007
#define ID_ANI_FPP_RIGHT_TOP_SHOOT 50008
#define ID_ANI_FPP_RIGHT_BOTTOM_SHOOT 50009

// SMALL
#define ID_ANI_FPP_SMALL_LEFT_TOP 50010
#define ID_ANI_FPP_SMALL_LEFT_BOTTOM 50011
#define ID_ANI_FPP_SMALL_LEFT_TOP_SHOOT 50012
#define ID_ANI_FPP_SMALL_LEFT_BOTTOM_SHOOT 50013
#define ID_ANI_FPP_SMALL_RIGHT_TOP 50014
#define ID_ANI_FPP_SMALL_RIGHT_BOTTOM 50015
#define ID_ANI_FPP_SMALL_RIGHT_TOP_SHOOT 50016
#define ID_ANI_FPP_SMALL_RIGHT_BOTTOM_SHOOT 50017


//DISTANCE
#define DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO 75
#define DISTANCE_SAFE_ZONE 25

// SPEED
#define FPP_SPEED 0.02f

// MODEL
#define FPP_BIG 1
#define FPP_SMALL 2

//  POSITION OF MARIO
#define LEFT_TOP_SIDE_NEAR  1
#define LEFT_TOP_SIDE_FAR  2
#define LEFT_BOTTOM_SIDE_NEAR  3
#define LEFT_BOTTOM_SIDE_FAR  4
#define RIGHT_TOP_SIDE_NEAR  5
#define RIGHT_TOP_SIDE_FAR  6
#define RIGHT_BOTTOM_SIDE_NEAR  7
#define RIGHT_BOTTOM_SIDE_FAR 8

// TIME
#define FPP_UP_TIME_OUT 6000
#define FPP_DOWN_TIME_OUT 5000
class CFirePiranhaPlant : public CGameObject
{
protected:
	float ax;
	float ay;

	float minY;
	float startY;
	int marioRange;

	ULONGLONG down_start;
	ULONGLONG up_start;
	BOOLEAN isMarioSafeZone;
	BOOLEAN startShoot, startDown;
	BOOLEAN isBottom, isFar;
	vector<LPGAMEOBJECT> ListFire;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);
	bool GetSafeZone();
	void ShootFire();
public:
	CFirePiranhaPlant(float x, float y, int model);
	void GetMarioRangeCurrent();
};

