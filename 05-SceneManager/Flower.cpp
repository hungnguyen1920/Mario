#include "Flower.h"

CFlower::CFlower(float x, float y) :CGameObject(x, y)
{

	this->ay = 0;
	this->ax = 0;
	this->x = x;
	this->y = y;

	minY = y - FLOWER_BBOX_HEIGHT;
}

void CFlower::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - FLOWER_BBOX_WIDTH / 2;
	top = y - FLOWER_BBOX_HEIGHT / 2;
	right = x + FLOWER_BBOX_WIDTH;
	bottom = y + FLOWER_BBOX_HEIGHT;
}

void CFlower::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CFlower::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;

	if (y <= minY)
	{
		vy = 0;
		y = minY;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CFlower::Render()
{
	int aniId = ID_ANI_FLOWER;

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
}

void CFlower::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FLOWER_STATE_UP:
		vy = -FLOWER_SPEED_UP;
		break;
	}
}

