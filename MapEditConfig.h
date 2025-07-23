#pragma once

struct MapEditConfig
{
	int MAP_WIDTH;
	int MAP_HEIGHT;
	int MAP_IMAGE_SIZE;
	int LEFT_MARGIN; // 画面左からのマージン
	int TOP_MARGIN; // 画面上からのマージン
};

const MapEditConfig& GetMapEditConfig();