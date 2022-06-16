#include "MushRoom.h"
#include "QuestionBrick.h"
#include "Leaf.h"
#include "MushRoom.h"

CMushRoom::CMushRoom(float x, float y, int model) :CGameObject(x, y)
{
	this->ay = 0;
	this->ax = 0;
	this->x = x;
	this->y = y;

	minY = y - MUSHROOM_BBOX_HEIGHT;
	this->model = model;
}

void CMushRoom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - MUSHROOM_BBOX_WIDTH / 2;
	top = y - MUSHROOM_BBOX_HEIGHT / 2;
	right = left + MUSHROOM_BBOX_WIDTH;
	bottom = top + MUSHROOM_BBOX_HEIGHT;
}

void CMushRoom::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CMushRoom::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CMushRoom*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

void CMushRoom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (y <= minY)
	{
		SetState(MUSHROOM_STATE_RUN);
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CMushRoom::Render()
{
	int aniId = -1;

	if (model == GREEN_MUSHROOM) {
		aniId = ID_ANI_GREEN_MUSHROOM;
	}
	else {
		aniId = ID_ANI_RED_MUSHROOM;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void CMushRoom::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case MUSHROOM_STATE_UP:
		vy = -MUSHROOM_SPEED_UP;
		break;
	case MUSHROOM_STATE_RUN:
		vx = MUSHROOM_RUN_SPEED;
		ay = MUSHROOM_GRAVITY;
		break;
	}
}
