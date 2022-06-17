#pragma once
#include "GameObject.h"

#define ID_ANI_HUD 300

class CHUD : public CGameObject
{
public:
	CHUD(float x, float y);
	virtual void Render();
	void Update(DWORD dt);
	void GetBoundingBox(float& l, float& t, float& r, float& b) { l = r = t = b = 0; }
};

