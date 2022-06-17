#include "QuestionBrick.h"
#include "Leaf.h"
#include "MushRoom.h"
#include "Mario.h"
#include "PlayScene.h"
#include "Flower.h"
#include "define.h"
#include "QBCoin.h"
CQuestionBrick::CQuestionBrick(float x, float y, int model) :CGameObject(x, y)
{

	this->model = model;

	this->ay = 0;
	this->minY = y - QUESTION_BRICK_BBOX_HEIGHT;
	this->startY = y;
	this->startX = x;

	SetType(EType::OBJECT);
}

void CQuestionBrick::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - QUESTION_BRICK_BBOX_WIDTH / 2;
	top = y - QUESTION_BRICK_BBOX_HEIGHT / 2;
	right = left + QUESTION_BRICK_BBOX_WIDTH;
	bottom = top + QUESTION_BRICK_BBOX_HEIGHT;
}

void CQuestionBrick::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};


void CQuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (y <= minY)
	{
		vy = QUESTION_BRICK_SPEED_DOWN;

	}
	if (y > startY)
	{
		y = startY;
		vy = 0;
		isEmpty = true;
		isUnbox = true;
	}

	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (isUnbox ) {
		if (model == QUESTION_BRICK_ITEM) {
			if (mario->GetLevel() == MARIO_LEVEL_SMALL) {
				CMushRoom* mushroom = new CMushRoom(x, y, RED_MUSHROOM);
				mushroom->SetState(MUSHROOM_STATE_UP);
				objects.push_back(mushroom);
			}
			else if (mario->GetLevel() == MARIO_LEVEL_BIG) {
				CLeaf* leaf = new CLeaf(x, y);
				leaf->SetState(LEAF_STATE_UP);
				objects.push_back(leaf);
			}
			else if (mario->GetLevel() == MARIO_LEVEL_RACCOON) {
				CFlower* flower = new CFlower(x, y);
				flower->SetState(FLOWER_STATE_UP);
				objects.push_back(flower);
			}
		}
		else {
			QBCoin* coin = new QBCoin(x, y);
			coin->SetState(QB_COIN_STATE_UP);
			objects.push_back(coin);
		}
		isUnbox = false;
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, coObjects);
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CQuestionBrick::Render()
{
	int aniId = ID_ANI_QUESTION_BRICK;

	if (isEmpty) {
		aniId = ID_ANI_QUESTION_BRICK_EMPTY;
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Render();
	}

	CAnimations::GetInstance()->Get(aniId)->Render(x, y);
	RenderBoundingBox();
}

void CQuestionBrick::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	}
}
