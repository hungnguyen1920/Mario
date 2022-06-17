#include "Leaf.h"

CLeaf::CLeaf(float x, float y) :CGameObject(x, y)
{

	this->ay = 0;
	this->ax = 0;
	this->x = x;
	this->y = y;

	minY = y - LEAF_MAX_HEIGHT;
	limitLeft = x;
	limitRight = x + LEAF_MAX_RIGHT;
}

void CLeaf::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - LEAF_BBOX_WIDTH / 2;
	top = y - LEAF_BBOX_HEIGHT / 2;
	right = x + LEAF_BBOX_WIDTH;
	bottom = y + LEAF_BBOX_HEIGHT;
}

void CLeaf::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CLeaf::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (y <= minY)
	{
		y = minY;
		isFall = true;
	}
	if (isFall) {
		vy = LEAF_GRAVITY;
		if (x <= limitLeft)
		{
			x = limitLeft;
			vx = LEAF_SPEED_X;
		}
		if (x >= limitRight)
		{
			x = limitRight;
			vx = -LEAF_SPEED_X;
		}
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CLeaf::Render()
{
	int aniId = -1;
	if (vx > 0) {
		aniId = ID_ANI_LEAF_RIGHT;
	}
	else {
		aniId = ID_ANI_LEAF_LEFT;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}

void CLeaf::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case LEAF_STATE_UP:
		vy = -LEAF_SPEED_UP;
		break;
	}
}

