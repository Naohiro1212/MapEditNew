#pragma once

struct MapEditConfig
{
	int MAP_WIDTH;
	int MAP_HEIGHT;
	int MAP_IMAGE_SIZE;
	int LEFT_MARGIN; // ��ʍ�����̃}�[�W��
	int TOP_MARGIN; // ��ʏォ��̃}�[�W��
};

const MapEditConfig& GetMapEditConfig();