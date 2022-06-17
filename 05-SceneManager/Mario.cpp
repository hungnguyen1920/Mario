#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Koopas.h"
#include "Coin.h"
#include "Portal.h"
#include "FireBall.h"
#include "Collision.h"
#include "QuestionBrick.h"
#include "Leaf.h"
#include "MushRoom.h"
#include "Flower.h"
#include "define.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt + nx * powerStack * ax;

	//limit move x
	if (abs(vx) > MARIO_WALKING_SPEED) {
		if (!isRunning) {
			vx = nx * MARIO_WALKING_SPEED;
		}
		else {
			if (abs(vx) >= MARIO_RUNNING_SPEED) {
				if (powerStack < MARIO_POWER_FULL) {
					vx = nx * MARIO_RUNNING_SPEED;
				}
				else {
					vx = nx * MARIO_RUNNING_MAX_SPEED;
				}
			}
		}
	}

	if (vx < 0 && nx > 0 && !isWalking)
	{
		vx = 0;
		ax = 0;
	}
	if (vx > 0 && nx < 0 && !isWalking)
	{
		vx = 0;
		ax = 0;
	}

	//limit move y
	if (vy <= -MARIO_JUMP_SPEED_MAX && !isRunningMax) {
		vy = -MARIO_JUMP_SPEED_MAX;
		ay = MARIO_GRAVITY;
	}

	if (vy <= -MARIO_JUMP_RUN_SPEED_Y && isRunningMax) {
		vy = -MARIO_JUMP_RUN_SPEED_Y;
		ay = MARIO_GRAVITY;
	}
	//change direction when run max speed

		
	if (level == MARIO_LEVEL_RACCOON && vy > 0) {
		canFallSlow = true;
	}

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	if (vy < 0) {
		isOnPlatform = false;
	}

	if (GetTickCount64() - running_start > POWER_STACK_TIME && isRunning && !isHoldTurtle)
	{
		running_start = GetTickCount64();
		powerStack++;
		if (powerStack > MARIO_POWER_FULL)
		{
			powerStack = MARIO_POWER_FULL;
			isRunningMax = true;
		}
		DebugOut(L"[INFO] powerStack! %d \n", powerStack);
	}

	if (GetTickCount64() - running_stop > POWER_STACK_LOST_TIME && powerStack && !isRunning)
	{
		running_stop = GetTickCount64();
		isRunningMax = false;
		powerStack--;
		if (powerStack <= 0)
		{
			powerStack = 0;
		}
		DebugOut(L"[INFO] powerStack! %d \n", powerStack);
	}

	if (GetTickCount64() - flying_start > LIMIT_MARIO_RACCOON_FLY_TIME && isFlying)
	{
		isFlying = false;
		isFlapping = false;
		canFallSlow = true;
		DebugOut(L"[INFO] mario raccoon fly time end\n");
	}

	if (GetTickCount64() - kick_start > MARIO_KICK_TIMEOUT && isKicking) {
		isKicking = false;
		kick_start = -1;
	}

	if (GetTickCount64() - transform_start > MARIO_TRANSFORM_TIME_OUT && isTransform)
	{
		isTransform = false;
		transform_start = -1;
	}

	if (isTransform) {
		ay = MARIO_GRAVITY;
		vx = 0;
	}

	if (isAdjustHeight) {
		y -= ADJUST_HEIGHT_MARIO_SMALL_TRANSFORM_BIG;
		isAdjustHeight = false;
	}

	if (GetTickCount64() - shoot_start > MARIO_FIRE_TIME_SHOOT_EFFECT && canShoot) {
		shoot_start = -1;
		canShoot = false;
	}

	if (isShooting && level == MARIO_LEVEL_FIRE)
	{
		if (ListFire.size() < MARIO_FIRE_BALL_LIMIT)
		{
			ShootFire();
			canShoot = true;
			isShooting = false;
		}
	}

	for (size_t i = 0; i < ListFire.size(); i++)
	{
		ListFire[i]->Update(dt, coObjects);
		if (ListFire[i]->GetState() == FIRE_BALL_DISAPPEAR) {
			ListFire.erase(ListFire.begin() + i);
		}
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);

}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{	
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		if (e->ny < 0) {
			isOnPlatform = true;
			isFlying = false;
			canFallSlow = false;
			vy = 0;
		}
		else {
			vy = 0;
			ay = MARIO_GRAVITY;
		}
	}
	else if (e->nx != 0) {
		if (e->obj->GetType() == OBJECT) {
			SetState(MARIO_STATE_RELEASE_RUN);
		}
	}

	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CKoopas*>(e->obj))
		OnCollisionWithKoopas(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CLeaf*>(e->obj))
		OnCollisionWithLeaf(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<CQuestionBrick*>(e->obj))
		OnCollisionWithQuestionBrick(e);
	else if (dynamic_cast<CMushRoom*>(e->obj))
		OnCollisionWithMushRoom(e);
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			if (goomba->GetModel() == GOOMBA_BASE) {
				goomba->SetState(GOOMBA_STATE_DIE);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
			}
			else
				if (state != GOOMBA_STATE_WALKING) {
					goomba->SetState(GOOMBA_STATE_WALKING);
					vy = -MARIO_JUMP_DEFLECT_SPEED;
				}
				else {
					goomba->SetState(GOOMBA_STATE_DIE);
					vy = -MARIO_JUMP_DEFLECT_SPEED;
				}
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					SetState(MARIO_STATE_DIE);
				}
			}
		}
	}
}

void CMario::OnCollisionWithKoopas(LPCOLLISIONEVENT e)
{
	CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);

	if (e->ny < 0)
	{
		if (koopas->GetState() == KOOPAS_STATE_WALKING || koopas->GetState() == KOOPAS_STATE_IS_KICKED)
		{
			koopas->SetState(KOOPAS_STATE_DEFEND);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
		else if (koopas->GetState() == KOOPAS_STATE_JUMP) {
			koopas->SetState(KOOPAS_STATE_WALKING);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
		else if (koopas->GetState() == KOOPAS_STATE_DEFEND)
		{
			koopas->SetState(KOOPAS_STATE_IS_KICKED);
		}
	}
	else if (e->nx != 0)
	{
		if (koopas->GetState() == KOOPAS_STATE_DEFEND || koopas->GetState() == KOOPAS_STATE_UPSIDE) {
			if (isRunning && !isHoldTurtle) {
				isHoldTurtle = true;
				isKicking = false;
				powerStack = 0;
			}
			else {
				SetState(MARIO_STATE_KICK);
				koopas->SetState(KOOPAS_STATE_IS_KICKED);
			}
		}
		else {
			if (level > MARIO_LEVEL_SMALL)
			{
				level--;
				StartUntouchable();
			}
			else
			{
				DebugOut(L">>> Mario DIE >>> \n");
				SetState(MARIO_STATE_DIE);
			}
		}
	}
	else if (e->ny > 0) {
		if (level > MARIO_LEVEL_SMALL)
		{
			level = MARIO_LEVEL_SMALL;
			StartUntouchable();
		}
		else
		{
			DebugOut(L">>> Mario DIE >>> \n");
			SetState(MARIO_STATE_DIE);
		}
	}
}

void CMario::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e) {
	CQuestionBrick* questionBrick = dynamic_cast<CQuestionBrick*>(e->obj);
	if (e->ny > 0 && !questionBrick -> isEmpty) {
		questionBrick->SetState(QUESTION_BRICK_STATE_UP);
	}
}

void CMario::OnCollisionWithLeaf(LPCOLLISIONEVENT e)
{
	level = MARIO_LEVEL_RACCOON;
	e->obj->Delete();
}
void CMario::OnCollisionWithMushRoom(LPCOLLISIONEVENT e)
{
	CMushRoom* mushroom = dynamic_cast<CMushRoom*>(e->obj);

	if (e->obj->GetModel() == RED_MUSHROOM) {
		transform_start = GetTickCount64();
		isTransform = true;
		isAdjustHeight = true;
		level = MARIO_LEVEL_BIG;
		e->obj->Delete();
	}
	
}

void CMario::OnCollisionWithFlower(LPCOLLISIONEVENT e)
{
	level = MARIO_LEVEL_FIRE;
	e->obj->Delete();
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	coin++;
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (isRunningMax)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
		}
		else
		{
			if (vy < 0) {
				if (isHoldTurtle) {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_HOLD_JUMP_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_HOLD_JUMP_LEFT;
				}
				else {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
				}
			}
			else {
				if (isHoldTurtle) {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_HOLD_JUMP_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_HOLD_JUMP_LEFT;
				}
				else {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_SMALL_FALL_RIGHT;
					else
						aniId = ID_ANI_MARIO_SMALL_FALL_LEFT;
				}
			}
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_SIT_LEFT;
		}
		else if (isKicking) {
			if (nx > 0)
				aniId = ID_ANI_MARIO_SMALL_KICK_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_KICK_LEFT;
		}
		else if (isHoldTurtle) {
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_LEFT;
			}
			else if (vx > 0) {
				aniId = ID_ANI_MARIO_SMALL_HOLD_WALK_RIGHT;
			}
			else {
				aniId = ID_ANI_MARIO_SMALL_HOLD_WALK_LEFT;
			}
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (nx < 0)
					aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_SMALL_RUN_MAX_RIGHT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
				else
					aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (nx > 0)
					aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_SMALL_RUN_MAX_LEFT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
				else
					aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
			}

	if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;

	return aniId;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (isRunningMax)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
		}
		else
		{
			if (vy < 0) {
				if (isHoldTurtle) {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_HOLD_JUMP_RIGHT;
					else
						aniId = ID_ANI_MARIO_HOLD_JUMP_LEFT;
				}
				else {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
					else
						aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
				}
				
			}
			else {
				if (isHoldTurtle) {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_HOLD_JUMP_RIGHT;
					else
						aniId = ID_ANI_MARIO_HOLD_JUMP_LEFT;
				}
				else {
				if (nx >= 0)
					aniId = ID_ANI_MARIO_FALL_RIGHT;
				else
					aniId = ID_ANI_MARIO_FALL_LEFT;
				}
			}
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_SIT_LEFT;
		}
		else if (isKicking) {
			if (nx > 0)
				aniId = ID_ANI_MARIO_KICK_RIGHT;
			else
				aniId = ID_ANI_MARIO_KICK_LEFT;
		}
		else if (isTransform) {
			if (nx > 0)
				aniId = ID_ANI_MARIO_TRANSFORM_RIGHT;
			else
				aniId = ID_ANI_MARIO_TRANSFORM_LEFT;

		}
		else if (isHoldTurtle) {
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_HOLD_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_HOLD_IDLE_LEFT;
			}
			else if (vx > 0) {
				aniId = ID_ANI_MARIO_HOLD_WALK_RIGHT;
			}
			else {
				aniId = ID_ANI_MARIO_HOLD_WALK_LEFT;
			}
		}
		else
		{
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (nx < 0)
					aniId = ID_ANI_MARIO_BRACE_RIGHT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_RUN_MAX_RIGHT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_RUNNING_RIGHT;
				else
					aniId = ID_ANI_MARIO_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (nx > 0)
					aniId = ID_ANI_MARIO_BRACE_LEFT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_RUN_MAX_LEFT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_RUNNING_LEFT;
				else
					aniId = ID_ANI_MARIO_WALKING_LEFT;
			}
		}

	if (aniId == -1) aniId = ID_ANI_MARIO_IDLE_RIGHT;

	return aniId;
}

//
// Get animdation ID for RACCOON Mario
//
int CMario::GetAniIdRaccoon()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (isFlying && isFlapping) {
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_FLY_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_FLY_LEFT;
		}
		else if (isRunningMax)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT;
		}
		else
		{
			if (isHoldTurtle) {
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_HOLD_JUMP_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_HOLD_JUMP_LEFT;
			}
			else {
				if (isHoldTurtle) {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_RACCOON_HOLD_JUMP_RIGHT;
					else
						aniId = ID_ANI_MARIO_RACCOON_HOLD_JUMP_LEFT;
				}
				else {
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT;
				}
			}
		}
		if (vy > 0) {
			if (!isFlying) {
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_FALL_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_FALL_LEFT;
			}
			else {
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_FALL_FLY_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_FALL_FLY_LEFT;
			}

			if (isFallSlowing) {
				if (nx >= 0)
					aniId = ID_ANI_MARIO_RACCOON_FALL_SLOW_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_FALL_SLOW_LEFT;
			}
		}

	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_RACCOON_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_SIT_LEFT;
		}
		else if (isKicking) {
			if (nx > 0)
				aniId = ID_ANI_MARIO_RACCOON_KICK_RIGHT;
			else
				aniId = ID_ANI_MARIO_RACCOON_KICK_LEFT;
		}
		else if (isHoldTurtle) {
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_RACCOON_HOLD_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_RACCOON_HOLD_IDLE_LEFT;
			}
			else if (vx > 0) {
				aniId = ID_ANI_MARIO_RACCOON_HOLD_WALK_RIGHT;
			}
			else {
				aniId = ID_ANI_MARIO_RACCOON_HOLD_WALK_LEFT;
			}
		}
		else
		{
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_RACCOON_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_RACCOON_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (nx < 0)
					aniId = ID_ANI_MARIO_RACCOON_BRACE_RIGHT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_RACCOON_RUN_MAX_RIGHT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_RACCOON_RUNNING_RIGHT;
				else
					aniId = ID_ANI_MARIO_RACCOON_WALKING_RIGHT;

			}
			else // vx < 0
			{
				if (nx > 0)
					aniId = ID_ANI_MARIO_RACCOON_BRACE_LEFT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_RACCOON_RUN_MAX_LEFT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_RACCOON_RUNNING_LEFT;
				else
					aniId = ID_ANI_MARIO_RACCOON_WALKING_LEFT;
			}
		}

	if (aniId == -1)
		aniId = ID_ANI_MARIO_RACCOON_IDLE_RIGHT;

	return aniId;
}

//
// Get animdation ID for FIRE Mario
//
int CMario::GetAniIdFire()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (isRunningMax)
		{
			if (nx >= 0)
				aniId = ID_ANI_MARIO_FIRE_JUMP_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_FIRE_JUMP_RUN_LEFT;
		}
		else
		{
			if (vy < 0) {
				if (isHoldTurtle) {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_FIRE_HOLD_JUMP_RIGHT;
					else
						aniId = ID_ANI_MARIO_FIRE_HOLD_JUMP_LEFT;
				}
				else {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_FIRE_JUMP_WALK_RIGHT;
					else
						aniId = ID_ANI_MARIO_FIRE_JUMP_WALK_LEFT;
				}
			}
			else {
				if (isHoldTurtle) {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_FIRE_HOLD_JUMP_RIGHT;
					else
						aniId = ID_ANI_MARIO_FIRE_HOLD_JUMP_LEFT;
				}
				else {
					if (nx >= 0)
						aniId = ID_ANI_MARIO_FIRE_FALL_RIGHT;
					else
						aniId = ID_ANI_MARIO_FIRE_FALL_LEFT;
				}
				
			}
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_FIRE_SIT_RIGHT;
			else
				aniId = ID_ANI_MARIO_FIRE_SIT_LEFT;
		}
		else if (isKicking) {
			if (nx > 0)
				aniId = ID_ANI_MARIO_FIRE_KICK_RIGHT;
			else
				aniId = ID_ANI_MARIO_FIRE_KICK_LEFT;
		}
		else if (canShoot) {
			if (nx > 0) {
				aniId = ID_ANI_MARIO_FIRE_SHOOT_FIRE_RIGHT;
			}
			else
				aniId = ID_ANI_MARIO_FIRE_SHOOT_FIRE_LEFT;
		}
		else if (isHoldTurtle) {
			if (vx == 0)
			{
				if (nx > 0) aniId = ID_ANI_MARIO_FIRE_HOLD_IDLE_RIGHT;
				else aniId = ID_ANI_MARIO_FIRE_HOLD_IDLE_LEFT;
			}
			else if (vx > 0) {
				aniId = ID_ANI_MARIO_FIRE_HOLD_WALK_RIGHT;
			}
			else {
				aniId = ID_ANI_MARIO_FIRE_HOLD_WALK_LEFT;
			}
		}
		else
		{
			if (vx == 0)
			{
				if (nx > 0)
					aniId = ID_ANI_MARIO_FIRE_IDLE_RIGHT;
				else
					aniId = ID_ANI_MARIO_FIRE_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (nx < 0)
					aniId = ID_ANI_MARIO_FIRE_BRACE_RIGHT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_FIRE_RUN_MAX_RIGHT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_FIRE_RUNNING_RIGHT;
				else
					aniId = ID_ANI_MARIO_FIRE_WALKING_RIGHT;
			}
			else // vx < 0
			{
				if (nx > 0)
					aniId = ID_ANI_MARIO_FIRE_BRACE_LEFT;
				else if (isRunningMax)
					aniId = ID_ANI_MARIO_FIRE_RUN_MAX_LEFT;
				else if (isRunning)
					aniId = ID_ANI_MARIO_FIRE_RUNNING_LEFT;
				else
					aniId = ID_ANI_MARIO_FIRE_WALKING_LEFT;
			}
		}

	if (aniId == -1)
		aniId = ID_ANI_MARIO_FIRE_IDLE_RIGHT;

	return aniId;
}

void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	if (state == MARIO_STATE_DIE)
		aniId = ID_ANI_MARIO_DIE;
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();
	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();
	else if (level == MARIO_LEVEL_RACCOON)
		aniId = GetAniIdRaccoon();
	else if (level == MARIO_LEVEL_FIRE)
		aniId = GetAniIdFire();

	for (int i = 0; i < ListFire.size(); i++)
	{
		ListFire[i]->Render();
	}
	animations->Get(aniId)->Render(x, y);

	//RenderBoundingBox();
	
	DebugOutTitle(L"Coins: %d", coin);
}

void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return;

	switch (state)
	{
	case MARIO_STATE_RUNNING_RIGHT:
		running_start = GetTickCount64();
		nx = 1;
		isRunning = true;
		break;
	case MARIO_STATE_RUNNING_LEFT:
		running_start = GetTickCount64();
		nx = -1;
		isRunning = true;
		break;
	case MARIO_STATE_RELEASE_RUN:
		isRunning = false;
		running_stop = GetTickCount64();
		break;
	case MARIO_STATE_WALKING_RIGHT:
		ax = MARIO_ACCEL_WALK_X;
		isWalking = true;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		ax = -MARIO_ACCEL_WALK_X;
		isWalking = true;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:

		if (isOnPlatform) {
			if (vy > -MARIO_JUMP_SPEED_MIN)
			{
				vy = -MARIO_JUMP_SPEED_MIN;
			}
			if (isRunningMax)
			{
				vy = -MARIO_JUMP_RUN_SPEED_Y;

				if (level == MARIO_LEVEL_RACCOON) {
					isFlying = true;
					flying_start = GetTickCount64();
				}
			}
			ay = -MARIO_ACCEL_JUMP_Y;
		}
		break;
	case MARIO_RACCOON_STATE_FLAPPING:
		ay = -MARIO_RACCOON_FLAPPING_SPEED;
		isFlapping = true;
		break;
	case MARIO_RACCOON_STATE_FALL_SLOW:
		isFallSlowing = true;
		vy = -MARIO_RACCOON_FALL_SLOW_SPEED;
		break;
	case MARIO_STATE_KICK:
		isKicking = true;
		kick_start = GetTickCount64();
		break;
	case MARIO_STATE_SIT:
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			isSitting = true;
			Decelerate();
			vy = 0.0f;
			y += MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;
	case MARIO_STATE_SHOOTING:
		isShooting = true;
		shoot_start = GetTickCount64();
		break;
	case MARIO_STATE_IDLE:
		Decelerate();
		ay = MARIO_GRAVITY;
		isWalking = false;
		break;

	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;
	}

	CGameObject::SetState(state);
}

void CMario::Decelerate()
{
	if (vx > 0)
	{
		ax = -MARIO_DECELERATE_SPEED;
	}
	if (vx < 0)
	{
		ax = MARIO_DECELERATE_SPEED;
	}
}

void CMario::ShootFire()
{
	CFireBall* fireBall = new CFireBall(x + ADJUST_MARIO_SHOOT_FIRE_X, y + ADJUST_MARIO_SHOOT_FIRE_Y);
	fireBall->SetState(FIRE_BALL_STATE_MARIO_SHOOT);
	ListFire.push_back(fireBall);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level != MARIO_LEVEL_SMALL)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else
		{
			left = x - MARIO_BIG_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH / 2;
		top = y - MARIO_SMALL_BBOX_HEIGHT / 2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	level = l;
}

