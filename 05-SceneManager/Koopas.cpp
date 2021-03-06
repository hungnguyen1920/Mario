
#include "Koopas.h"
#include "Game.h"
#include "Mario.h"
#include "PlayScene.h"
#include "Platform.h"
#include "Goomba.h"
#include "define.h"
#include "QuestionBrick.h"
CKoopas::CKoopas(float x, float y, int model) :CGameObject(x, y)
{
	this->ax = 0;
	this->ay = KOOPAS_GRAVITY;
	this->model = model;
	defend_start = -1;
	isHeld = false;
	if (model == KOOPAS_GREEN_WING) {
		SetState(KOOPAS_STATE_JUMP);
	}
	else {
		SetState(KOOPAS_STATE_WALKING);
	}

	SetType(EType::ENEMY);
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

	if (e->ny != 0)
	{
		vy = 0;
		if (model == KOOPAS_GREEN_WING && state == KOOPAS_STATE_JUMP) {
			vy = -KOOPAS_WING_JUMP_SPEED;
			ay = KOOPAS_WING_GRAVITY;
		}
	}
	else if (e->nx != 0)
	{
		if (e->obj->GetType() == EType::ENEMY) {
			if (state == KOOPAS_STATE_IS_KICKED) {
				e->obj->SetState(ENEMY_STATE_IS_ATTACKED);
			}
		}
		else if (e->obj->GetType() == EType::OBJECT) {
			vx = -vx;
		}
	}

	if (dynamic_cast<CPlatform*>(e->obj))
		OnCollisionWithPlatform(e);
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
		OnCollisionWithQuestionBrick(e);
}

int CKoopas::IsCollidable()
{
	if (state == ENEMY_STATE_IS_FIRE_ATTACKED) {
		return 0;
	}
	else {
		return 1;
	}
}

void CKoopas::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e)
{
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);

	if (e->nx != 0 && !questionBrick->isEmpty) {
		if (state == KOOPAS_STATE_IS_KICKED) {
			questionBrick->SetState(QUESTION_BRICK_STATE_UP);
		}
	}
}

void CKoopas::OnCollisionWithPlatform(LPCOLLISIONEVENT e)
{
	CPlatform* platform = dynamic_cast<CPlatform*>(e->obj);

	if (e->ny < 0) {
		if (state == KOOPAS_STATE_WALKING && model == KOOPAS_RED) {
			if (x <= e->obj->GetX() - ADJUST_X_TO_RED_CHANGE_DIRECTION)
			{
				vy = 0;
				vx = KOOPAS_WALKING_SPEED;
			}
			else if (x + KOOPAS_BBOX_WIDTH >= e->obj->GetX() + 520) {
				vy = 0;
				vx = -KOOPAS_WALKING_SPEED;
			}
		}
	}
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;
	
	CGameObject::SetState(state);
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (mario->isHoldTurtle) {
		if (mario->GetDirection() > 0) {
			x = mario->GetX() + ADJUST_POSITION_KOOPAS_HELD;
			y = mario->GetY();
		}
		else {
			x = mario->GetX() - ADJUST_POSITION_KOOPAS_HELD;
			y = mario->GetY();
		}
		ay = 0;
		isHeld = true;
	}
	else {
		if (isHeld) {
			SetState(KOOPAS_STATE_IS_KICKED);
		}
	}

	// start animation comeback

	if (GetTickCount64() - defend_start > KOOPAS_COMBACK_START && (isDefend || isUpside ) && !isKicked) {
		isComeback = true;
	}

	// end defend and start walking
	if (GetTickCount64() - defend_start > KOOPAS_DEFEND_TIMEOUT && (isDefend || isUpside) && !isKicked) {
		SetState(KOOPAS_STATE_WALKING);
		defend_start = -1;
		vy = -KOOPAS_COMBACK_HEIGHT_ADJUST;
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CKoopas::Render()
{
	int aniId = ID_ANI_KOOPAS_WALKING_RIGHT;

	if (model == KOOPAS_GREEN_WING) {
		if (vx > 0) {
			aniId = ID_ANI_KOOPAS_GREEN_WING_RIGHT;
		}
		else {
			aniId = ID_ANI_KOOPAS_GREEN_WING_LEFT;
		}
	}

	if (model == KOOPAS_GREEN || model == KOOPAS_GREEN_WING)
	{
		if (state == ENEMY_STATE_IS_FIRE_ATTACKED) {
			aniId = ID_ANI_KOOPAS_IS_UPSIDE;
		}
		if (vx > 0)
		{
			if (state == KOOPAS_STATE_WALKING)
			{
				aniId = ID_ANI_KOOPAS_WALKING_RIGHT;
			}
			else if (isKicked)
			{
				if (isDefend)
				{
					aniId = ID_ANI_KOOPAS_IS_KICKED;
				}
				else if (isUpside)
				{
					aniId = ID_ANI_KOOPAS_UPSIDE_ISKICKED;
				}
			}
		}
		else
		{
			if (state == KOOPAS_STATE_WALKING)
			{
				aniId = ID_ANI_KOOPAS_WALKING_LEFT;
			}
			else if (isKicked)
			{
				if (isDefend)
				{
					aniId = ID_ANI_KOOPAS_IS_KICKED;
				}
				else if (isUpside)
				{
					aniId = ID_ANI_KOOPAS_UPSIDE_ISKICKED;
				}
			}
		}
		if (!isKicked)
		{
			if (isDefend)
			{
				if (isComeback)
				{
					aniId = ID_ANI_KOOPAS_COMEBACK;
				}
				else
				{
					aniId = ID_ANI_KOOPAS_DEFEND;
				}
			}
			else if (isUpside)
			{
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
	}
	else if (model == KOOPAS_RED)
	{
		if (vx > 0)
		{
			if (state == KOOPAS_STATE_WALKING)
			{
				aniId = ID_ANI_KOOPAS_RED_WALKING_RIGHT;
			}
			else if (isKicked)
			{
				if (isDefend)
				{
					aniId = ID_ANI_KOOPAS_RED_IS_KICKED;
				}
				else if (isUpside)
				{
					aniId = ID_ANI_KOOPAS_RED_UPSIDE_ISKICKED;
				}
			}
		}
		else
		{
			if (state == KOOPAS_STATE_WALKING)
			{
				aniId = ID_ANI_KOOPAS_RED_WALKING_LEFT;
			}
			else if (isKicked)
			{
				if (isDefend)
				{
					aniId = ID_ANI_KOOPAS_RED_IS_KICKED;
				}
				else if (isUpside)
				{
					aniId = ID_ANI_KOOPAS_RED_UPSIDE_ISKICKED;
				}
			}
		}
		if (!isKicked)
		{
			if (isDefend)
			{
				if (isComeback)
				{
					aniId = ID_ANI_KOOPAS_RED_COMEBACK;
				}
				else
				{
					aniId = ID_ANI_KOOPAS_RED_DEFEND;
				}
			}
			else if (isUpside)
			{
				if (isComeback)
				{
					aniId = ID_ANI_KOOPAS_RED_UPSIDE_COMEBACK;
				}
				else
				{
					aniId = ID_ANI_KOOPAS_RED_IS_UPSIDE;
				}
			}
		}
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	CMario * mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	switch (state)
	{
	case KOOPAS_STATE_WALKING:
		vx = -KOOPAS_WALKING_SPEED;
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
		isHeld = false;
		vx = mario->GetDirection() * KOOPAS_IS_KICKED_SPEED;
		break;
	case KOOPAS_STATE_JUMP:
		vx = 0.04f;
		isComeback = false;
		isDefend = false;
		isUpside = false;
		isKicked = false;
		break;
	case ENEMY_STATE_IS_FIRE_ATTACKED:
		vy = -0.3f;
		break;
	}

}
