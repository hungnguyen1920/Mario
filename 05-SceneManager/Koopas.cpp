
#include "Koopas.h"
#include "Game.h"
#include "Mario.h"
#include "PlayScene.h"

CKoopas::CKoopas(float x, float y) :CGameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPAS_GRAVITY;
	defend_start = -1;

	SetState(KOOPAS_STATE_WALKING);
}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isDefend || isUpside) {
		
		left = x - KOOPAS_BBOX_WIDTH / 2;
		top = y - KOOPAS_BBOX_HEIGHT_DEFEND / 2;
		right = left + KOOPAS_BBOX_WIDTH;
		bottom = top + KOOPAS_BBOX_HEIGHT_DEFEND;
		
	}
	else {
		left = x - KOOPAS_BBOX_WIDTH / 2;
		top = y - KOOPAS_BBOX_HEIGHT / 2;
		right = left + KOOPAS_BBOX_WIDTH;
		bottom = top + KOOPAS_BBOX_HEIGHT;
	}
}

void CKoopas::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CKoopas::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CKoopas*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	// start animation comeback

	if (GetTickCount64() - defend_start > KOOPAS_COMBACK_START && (isDefend || isUpside ) && !isKicked) {
		isComeback = true;
	}

	// end defend and start walking
	if (GetTickCount64() - defend_start > KOOPAS_DEFEND_TIMEOUT && (isDefend || isUpside) && !isKicked) {
		SetState(KOOPAS_STATE_WALKING);
		defend_start = -1;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CKoopas::Render()
{
	int aniId = -1;
	
		if (vx > 0) {
			if (state == KOOPAS_STATE_WALKING) {
				aniId = ID_ANI_KOOPAS_WALKING_RIGHT;
			} 
			else if (isKicked) {
				if (isDefend) {
					aniId = ID_ANI_KOOPAS_IS_KICKED;
				} 
				else if (isUpside) {
					aniId = ID_ANI_KOOPAS_UPSIDE_ISKICKED;
				}
			}
		}
		else {
			if (state == KOOPAS_STATE_WALKING) {
				aniId = ID_ANI_KOOPAS_WALKING_LEFT;
			}
			else if (isKicked) {
				if (isDefend) {
					aniId = ID_ANI_KOOPAS_IS_KICKED;
				}
				else if (isUpside) {
					aniId = ID_ANI_KOOPAS_UPSIDE_ISKICKED;
				}
			}
		}
		if (!isKicked) {
			if(isDefend) {
				if (isComeback)
				{
					aniId = ID_ANI_KOOPAS_COMEBACK;
				}
				else
				{
					aniId = ID_ANI_KOOPAS_DEFEND;
				}
			}
			else if (isUpside) {
				if (isComeback)
				{
					aniId = ID_ANI_KOOPAS_UPSIDE_COMEBACK;
				}
				else
				{
					aniId = ID_ANI_KOOPAS_IS_UPSIDE;
				}
			}
			
		}
			
	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	CMario * mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	mario_nx = mario->GetDirection();
	switch (state)
	{
	case KOOPAS_STATE_WALKING:
		vx = KOOPAS_WALKING_SPEED;
		isComeback = false;
		isDefend = false;
		isUpside = false;
		isKicked = false;
		break;
	case KOOPAS_STATE_DEFEND:
		isDefend = true;
		isComeback = false;
		isKicked = false;
		isUpside = false;
		defend_start = GetTickCount64();
		vx = 0;
		break;
	case KOOPAS_STATE_UPSIDE:
		isUpside = true;
		isDefend = false;
		isComeback = false;
		isKicked = false;
		defend_start = GetTickCount64();
		vx = 0;
		break;
	case KOOPAS_STATE_IS_KICKED:
		isKicked = true;
		vx = mario_nx*KOOPAS_IS_KICKED_SPEED;
		break;
	}

}
