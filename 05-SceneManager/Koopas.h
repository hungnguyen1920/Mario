#pragma once
#include "GameObject.h"

#define KOOPAS_GRAVITY 0.002f
#define KOOPAS_WALKING_SPEED 0.02f
#define KOOPAS_IS_KICKED_SPEED 0.1f


#define KOOPAS_BBOX_WIDTH 16
#define KOOPAS_BBOX_HEIGHT 26
#define KOOPAS_BBOX_HEIGHT_DEFEND 16

#define KOOPAS_DEFEND_TIMEOUT 8000 // thời gian hết thời gian ( ra khỏi mai rùa và bắt đầu đi)
#define KOOPAS_COMBACK_START 5000 //thời gian tính từ lúc defend đến lúc có hiệu ứng comeback

#define KOOPAS_STATE_WALKING 100
#define KOOPAS_STATE_DEFEND 200
#define KOOPAS_STATE_IS_KICKED 300
#define KOOPAS_STATE_UPSIDE 400

#define ID_ANI_KOOPAS_WALKING_RIGHT 40001
#define ID_ANI_KOOPAS_WALKING_LEFT 40002
#define ID_ANI_KOOPAS_DEFEND 40003
#define ID_ANI_KOOPAS_IS_KICKED 40004
#define ID_ANI_KOOPAS_IS_UPSIDE 40005
#define ID_ANI_KOOPAS_COMEBACK 40006
#define ID_ANI_KOOPAS_UPSIDE_ISKICKED 40007	// hiệu ứng bị raccoon tấn công và bị đá
#define ID_ANI_KOOPAS_UPSIDE_COMEBACK 40008 // hiệu ứng bị lật úp sau đó bắt đầu ra khỏi mai rùa

class CKoopas : public CGameObject
{
protected:
	float ax;
	float ay;

	DWORD defend_start;

	BOOLEAN isDefend;
	BOOLEAN isKicked;
	BOOLEAN isComeback;
	BOOLEAN isUpside;

	int mario_nx;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CKoopas(float x, float y);
	virtual void SetState(int state);
};