#include "QBCoin.h"
QBCoin::QBCoin(float x, float y) : CGameObject(x, y)
{

	this->ay = 0;
	this->ax = 0;
	this->x = x;
	this->y = y;

	minY = y - QB_COIN_MAX_HEIGHT;
	heightFinish = y - HEIGHT_FINISH;
}

void QBCoin::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void QBCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (y <= minY)
	{
		y = minY;
		SetState(QB_COIN_STATE_DOWN);
		isFall = true;
	}
	if (y >= heightFinish && isFall)
	{
		isFinish = true;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void QBCoin::Render()
{
	int aniId = ID_ANI_QB_COIN;
	if (isFinish) return;
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}

void QBCoin::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case QB_COIN_STATE_UP:
		vy = -QB_COIN_SPEED;
		break;
	case QB_COIN_STATE_DOWN:
		vy = QB_COIN_SPEED;
		break;
	}
}
