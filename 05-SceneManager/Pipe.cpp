#include "Pipe.h"


CPipe::CPipe(float x, float y, int model) : CGameObject(x, y) {

	this->x = x;
	this->y = y;
	this->model = model;
}

void CPipe::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - PIPE_BBOX_WIDTH / 2;
	right = left + PIPE_BBOX_WIDTH;

	if (model == PIPE_LONG) {
		top = y - PIPE_LONG_BBOX_HEIGHT / 2;
		bottom = top + PIPE_LONG_BBOX_HEIGHT;
	}
	else if (model == PIPE_MEDIUM) {
		top = y - PIPE_MEDIUM_BBOX_HEIGHT / 2;
		bottom = top + PIPE_MEDIUM_BBOX_HEIGHT;
	}
}


void CPipe::Render()
{
	int aniId = ID_ANI_PIPE_LONG;

	if (model == PIPE_MEDIUM) {
		aniId = ID_ANI_PIPE_MEDIUM;
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}