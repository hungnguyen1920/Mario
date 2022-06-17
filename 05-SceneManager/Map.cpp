#include "Map.h"
#include <fstream>
#include <iostream>

Map::Map(int TexID, int NumofRowMap, int NumofColMap, int NumofRowTileSet, int NumofColTileSet, int TotalTile)
{
	Tex = CTextures::GetInstance()->Get(TexID);
	this->NumofRowMap = NumofRowMap;
	this->NumofColMap = NumofColMap;
	this->NumofRowTileSet = NumofRowTileSet;
	this->NumofColTileSet = NumofColTileSet;
	this->TotalTile = TotalTile;
	CamX = CamY = 0;
}
Map::~Map() {}
void Map::SetMapData(int** mapData)
{
	TileMapData = mapData;
}
void Map::DrawMap()
{
	int FirstColumn = floor(CamX / TILE_WIDTH);
	int LastColumn = ceil((CamX + CGame::GetInstance()->GetScreenWidth()) / TILE_WIDTH);
	if (LastColumn >= NumofRowMap)
		LastColumn = NumofColMap - 1;
	for (int CurrentRow = 0; CurrentRow < NumofRowMap; CurrentRow++) {
		for (int CurrentColumn = 0; CurrentColumn <= LastColumn; CurrentColumn++)
		{
			int index = TileMapData[CurrentRow][CurrentColumn] - 1;
			if (index < TotalTile)
				Tiles.at(index)->Draw(CurrentColumn * TILE_WIDTH, CurrentRow * TILE_HEIGHT - HUD_HEIGHT);
		}
	}
}
void Map::GetSpriteTile()
{
	for (int id = 0; id < TotalTile; id++)
	{
		{
			int left = id % NumofColTileSet * TILE_WIDTH;
			int top = id / NumofColTileSet * TILE_HEIGHT;
			int right = left + TILE_WIDTH;
			int bottom = top + TILE_HEIGHT;
			LPSPRITE SpriteTile = new CSprite(id, left, top, right, bottom, Tex);
			Tiles.push_back(SpriteTile);
		}
	}
}
int Map::GetMapHeight()
{
	return NumofRowMap * TILE_HEIGHT;
}
int Map::GetMapWidth()
{
	return NumofColMap * TILE_WIDTH;
}


