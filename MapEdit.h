#pragma once
#include "Library\GameObject.h"
#include <vector>
#include "globals.h"
#include "MapEditConfig.h"

struct Info
{
	int x;
	int y;
	const char* text;
};

class MapEdit :
    public GameObject
{
public:
	MapEdit();
	~MapEdit();
	
	void SetMap(Point p, int value, int nowrow);
	int GetMap(Point p, int nowrow) const;
	bool IsInMapEditArea() const { return isInMapEditArea_; } //�}�b�v�G�f�B�^�̈���ɂ��邩�ǂ������擾����

	void Update() override;
	void Draw() override;
	void SaveMapData();
	void LoadMapData();
	void ChangeLayer();

	// �w��w���N���A����
	void ClearLayer();

private:
	std::vector<std::vector<int>> myMap_; //�}�b�v�̔z��
	Rect mapEditRect_; //�}�b�v�̈�̋�`
	Rect drawAreaRect_; //�`��̈�̋�`
	bool isInMapEditArea_; //�}�b�v�G�f�B�^�̈���ɂ��邩�ǂ���
	MapEditConfig efg_; // �}�b�v�G�f�B�b�g�̐ݒ��ێ�����
	int layer_;
};

