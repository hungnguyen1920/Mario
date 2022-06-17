#pragma once
#include "GameObject.h"

#define FLOWER_BBOX_WIDTH 16
#define FLOWER_BBOX_HEIGHT 16
#define ID_ANI_FLOWER 107	

#define FLOWER_STATE_UP 100
#define FLOWER_SPEED_UP 0.1f

class CFlower : public CGameObject
{
protected:
	float ax;
	float ay;

	float minY;
	float limitLeft, limitRight;

	BOOLEAN isFall = false;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 0; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

public:
	CFlower(float x, float y);
	virtual void SetState(int state);
};

