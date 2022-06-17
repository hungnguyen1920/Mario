#include "FirePiranhaPlant.h"
#include "Mario.h"
#include "PlayScene.h"
#include "FireBall.h"
#include "define.h"

CFirePiranhaPlant::CFirePiranhaPlant(float x, float y, int model) : CGameObject(x, y)
{
	this->x = x;
	this->y = y;
	this->model = model;
	this->ax = 0;
	this->ay = 0;
	this->startY = y;

	if (model == FPP_BIG) {
		this->minY = y - FPP_BIG_BBOX_HEIGHT;
	}
	else {
		this->minY = y - FPP_SMALL_BBOX_HEIGHT;
	}

	SetType(EType::ENEMY);
}

void CFirePiranhaPlant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (model == FPP_BIG) {
		left = x - FPP_BBOX_WIDTH / 2;
		top = y - FPP_BIG_BBOX_HEIGHT / 2;
		right = left + FPP_BBOX_WIDTH;
		bottom = top + FPP_BIG_BBOX_HEIGHT;
	}
	else {
		left = x - FPP_BBOX_WIDTH / 2;
		top = y - FPP_SMALL_BBOX_HEIGHT / 2;
		right = left + FPP_BBOX_WIDTH;
		bottom = top + FPP_SMALL_BBOX_HEIGHT;
	}
}

void CFirePiranhaPlant::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void CFirePiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (y <= minY) {
		y = minY;
		down_start = GetTickCount64();
		startShoot = true;
	}
	else if (y >= startY) {
		y = startY;
		up_start = GetTickCount64();
		startDown = true;
	}

	if (GetTickCount64() - up_start > FPP_UP_TIME_OUT && startShoot) {
		ShootFire();
		startShoot = false;
		vy = FPP_SPEED;
		up_start = 0;
	}

	if (GetTickCount64() - down_start > FPP_DOWN_TIME_OUT && startDown && !GetSafeZone()) {
		startDown = false;
		vy = -FPP_SPEED;
		down_start = 0;
	}

	for (size_t i = 0; i < ListFire.size(); i++)
	{
		ListFire[i]->Update(dt, coObjects);
		if (ListFire[i]->GetState() == FIRE_BALL_DISAPPEAR) {
			ListFire.erase(ListFire.begin() + i);
		}
	}
	GetMarioRangeCurrent();
	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CFirePiranhaPlant::Render()
{
	int aniId = ID_ANI_FPP_LEFT_BOTTOM;
	if (startShoot)
	{
		if (isBottom) {
			if (model == FPP_BIG) {
				if (nx > 0) {
					aniId = ID_ANI_FPP_RIGHT_BOTTOM_SHOOT;
				}
				else {
					aniId = ID_ANI_FPP_LEFT_BOTTOM_SHOOT;
				}
			}
			else {
				if (nx > 0) {
					aniId = ID_ANI_FPP_SMALL_RIGHT_BOTTOM_SHOOT;
				}
				else {
					aniId = ID_ANI_FPP_SMALL_LEFT_BOTTOM_SHOOT;
				}
			}
		}
		else {
			if (model == FPP_BIG) {
				if (nx > 0) {
					aniId = ID_ANI_FPP_RIGHT_TOP_SHOOT;
				}
				else {
					aniId = ID_ANI_FPP_LEFT_TOP_SHOOT;
				}
			}
			else {
				if (nx > 0) {
					aniId = ID_ANI_FPP_SMALL_RIGHT_TOP_SHOOT;
				}
				else {
					aniId = ID_ANI_FPP_SMALL_LEFT_TOP_SHOOT;
				}
			}
		}
	}
	else
	{
		if (isBottom) {
			if (model == FPP_BIG) {
				if (nx > 0) {
					aniId = ID_ANI_FPP_RIGHT_BOTTOM;
				}
				else {
					aniId = ID_ANI_FPP_LEFT_BOTTOM;
				}
			}
			else {
				if (nx > 0) {
					aniId = ID_ANI_FPP_SMALL_RIGHT_BOTTOM;
				}
				else {
					aniId = ID_ANI_FPP_SMALL_LEFT_BOTTOM;
				}
			}
		}
		else {
			if (model == FPP_BIG) {
				if (nx > 0) {
					aniId = ID_ANI_FPP_RIGHT_TOP;
				}
				else {
					aniId = ID_ANI_FPP_LEFT_TOP;
				}
			}
			else {
				if (nx > 0) {
					aniId = ID_ANI_FPP_SMALL_RIGHT_TOP;
				}
				else {
					aniId = ID_ANI_FPP_SMALL_LEFT_TOP;
				}
			}
		}
	}

	for (int i = 0; i < ListFire.size(); i++)
	{
		ListFire[i]->Render();
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	//RenderBoundingBox();
}

void CFirePiranhaPlant::GetMarioRangeCurrent()
{
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (mario->GetX() < this->x && mario->GetY() < this->y)
	{
		if (this->x - mario->GetX() < DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = false;
			isFar = false;
			nx = -1;
		}
		if (this->x - mario->GetX() >= DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = false;
			isFar = true;
			nx = -1;
		}
	}
	if (mario->GetX() < this->x && mario->GetY() > this->y)
	{
		if (this->x - mario->GetX() < DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = true;
			isFar = false;
			nx = -1;
		}
		if (this->x - mario->GetX() > DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = true;
			isFar = true;
			nx = -1;
		}

	}
	if (mario->GetX() > this->x && mario->GetY() < this->y)
	{
		if (this->x - mario->GetX() < DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = false;
			isFar = false;
			nx = 1;
		}
		if (this->x - mario->GetX() >= DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = false;
			isFar = true;
			nx = 1;
		}
	}
	if (mario->GetX() > this->x && mario->GetY() > this->y)
	{
		if (this->x - mario->GetX() < DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = true;
			isFar = false;
			nx = 1;
		}
		if (this->x - mario->GetX() >= DISTANCE_SHOOT_FIRE_ACCRODING_TO_MARIO)
		{
			isBottom = true;
			isFar = true;
			nx = 1;
		}
	}
}

bool CFirePiranhaPlant::GetSafeZone()
{
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (mario->GetX() < this->x)
	{
		if (this->x - mario->GetX() <= DISTANCE_SAFE_ZONE)
		{
			return true;
		}
	}
	if (mario->GetX() > this->x)
	{
		if (mario->GetX() - this->x <= DISTANCE_SAFE_ZONE)
		{
			return true;
		}
	}
	return false;
}

void CFirePiranhaPlant::ShootFire()
{
	CFireBall* fireBall = new CFireBall(x, y - ADJUST_FPP_SHOOT_FIRE_BALL_HEIGHT);
	int directionYFireball = 0;
	fireBall->SetDirectionX(nx);

	if (isBottom) {
		fireBall->SetDirectionY(1);
		if (isFar) {
			fireBall->SetState(FIRE_BALL_STATE_FPP_SHOOT_FAR);
		}
		else {
			fireBall->SetState(FIRE_BALL_STATE_FPP_SHOOT_NEAR);
		}
	}
	else {
		fireBall->SetDirectionY(-1);
		if (isFar) {
			fireBall->SetState(FIRE_BALL_STATE_FPP_SHOOT_FAR);
		}
		else {
			fireBall->SetState(FIRE_BALL_STATE_FPP_SHOOT_NEAR);
		}
	}
	ListFire.push_back(fireBall);
}
