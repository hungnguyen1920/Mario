#pragma once
#include "GameObject.h"

#define QB_COIN_MAX_HEIGHT 90
#define QB_COIN_SPEED 0.2f
#define HEIGHT_FINISH 30
#define ID_ANI_QB_COIN 112
#define QB_COIN_STATE_UP 100
#define QB_COIN_STATE_DOWN 200
class QBCoin : public CGameObject
{
protected:
	float ax;
	float ay;

	float minY;
	float heightFinish;

	BOOLEAN isFinish = false;
	BOOLEAN isFall = false;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) { left = top = right = bottom = 0; }
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void OnNoCollision(DWORD dt);
	virtual void Render();

public:
	QBCoin(float x, float y);
	virtual void SetState(int state);

};

