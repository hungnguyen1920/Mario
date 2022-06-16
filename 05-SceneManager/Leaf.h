#pragma once
#include "GameObject.h"

#define LEAF_BBOX_WIDTH 16
#define LEAF_BBOX_HEIGHT 14
#define LEAF_GRAVITY 0.02f;

#define ID_ANI_LEAF_RIGHT 110
#define ID_ANI_LEAF_LEFT 111

#define LEAF_SPEED_X 0.08f;
#define LEAF_SPEED_UP 0.1f;

#define LEAF_STATE_UP 100

#define LEAF_MAX_HEIGHT 50
#define LEAF_MAX_RIGHT 32

class CLeaf : public CGameObject
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
	CLeaf(float x, float y);
	virtual void SetState(int state);
};

