#include "HUD.h"
CHUD::CHUD(float x, float y)  : CGameObject(x, y) {

	this->x = x;
	this->y = y;
}

void CHUD::Update(DWORD dt) {
	
}

void CHUD::Render()
{
	CAnimations::GetInstance()->Get(ID_ANI_HUD)->Render(x, y);
}
