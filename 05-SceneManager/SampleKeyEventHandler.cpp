#include "SampleKeyEventHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"

void CSampleKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CMario* mario = (CMario *)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer(); 

	switch (KeyCode)
	{
	case DIK_DOWN:
		mario->SetState(MARIO_STATE_SIT);
		break;
	case DIK_1:
		mario->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		mario->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_3:
		mario->SetLevel(MARIO_LEVEL_RACCOON);
		break;
	case DIK_4:
		mario->SetLevel(MARIO_LEVEL_FIRE);
		break;
	case DIK_0:
		mario->SetState(MARIO_STATE_DIE);
		break;
	case DIK_S:
		if (mario->GetLevel() == MARIO_LEVEL_RACCOON) {
			if (mario->isFlying) {
				mario->isFlapping = true;
				mario->SetState(MARIO_RACCOON_STATE_FLAPPING);
			} 
			else if (mario->canFallSlow) {
				mario->SetState(MARIO_RACCOON_STATE_FALL_SLOW);
			}
		}
		mario->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_A:
		if (mario->GetLevel() == MARIO_LEVEL_FIRE) {
			if (!mario->isShooting ) {
				mario->SetState(MARIO_STATE_SHOOTING);
			}
		}
		break;
	case DIK_R: // reset
		//Reload();
		break;
	}
}

void CSampleKeyHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);

	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	switch (KeyCode)
	{
	case DIK_S:
		mario->isFallSlowing = false;
		break;
	case DIK_DOWN:
		mario->SetState(MARIO_STATE_SIT_RELEASE);
		break;
	case DIK_RIGHT:
		mario->SetState(MARIO_STATE_IDLE);
		break;
	case DIK_LEFT:
		mario->SetState(MARIO_STATE_IDLE);
		break;
	case DIK_A:
		mario->SetState(MARIO_STATE_RELEASE_RUN);
		if (mario->isHoldTurtle) {
		mario->isHoldTurtle = false;
		mario->SetState(MARIO_STATE_KICK);
		}
		break;
	}
}

void CSampleKeyHandler::KeyState(BYTE *states)
{
	LPGAME game = CGame::GetInstance();
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (!mario->isRunning && game->IsKeyDown(DIK_A))
		{
			mario->SetState(MARIO_STATE_RUNNING_RIGHT);
			mario->SetIsRunning(true);
		}
		else
		{
			mario->SetState(MARIO_STATE_WALKING_RIGHT);
		}

		if (mario->isSitting) {
			mario->SetState(MARIO_STATE_SIT_RELEASE);
		}
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (!mario->isRunning && game->IsKeyDown(DIK_A))
		{
			mario->SetState(MARIO_STATE_RUNNING_LEFT);
			mario->SetIsRunning(true);
		}
		else
		{
			mario->SetState(MARIO_STATE_WALKING_LEFT);
		}
		if (mario->isSitting) {
			mario->SetState(MARIO_STATE_SIT_RELEASE);
		}
	}
	else if (game->IsKeyDown(DIK_A))
	{
		
	}
	else if (game->IsKeyDown(DIK_S))
	{
		mario->SetState(MARIO_STATE_JUMP);
	}
	else
		mario->SetState(MARIO_STATE_IDLE);
}