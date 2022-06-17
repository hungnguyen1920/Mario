#pragma once

#define HUD_HEIGHT 32

#define TYPE_OBJECT 1
#define TYPE_ENEMY 2
#define TYPE_COIN 3
enum EType {
	OBJECT = TYPE_OBJECT,
	ENEMY = TYPE_ENEMY,
	COIN = TYPE_COIN,
};


#define ENEMY_STATE_IS_ATTACKED 900 // koopas is kicked attack or tail attack
#define ENEMY_STATE_IS_FIRE_ATTACKED 901