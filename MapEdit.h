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
	bool IsInMapEditArea() const { return isInMapEditArea_; } //マップエディタ領域内にいるかどうかを取得する

	void Update() override;
	void Draw() override;
	void SaveMapData();
	void LoadMapData();
	void ChangeLayer();

	// 指定層をクリアする
	void ClearLayer();

private:
	std::vector<std::vector<int>> myMap_; //マップの配列
	Rect mapEditRect_; //マップ領域の矩形
	Rect drawAreaRect_; //描画領域の矩形
	bool isInMapEditArea_; //マップエディタ領域内にいるかどうか
	MapEditConfig efg_; // マップエディットの設定を保持する
	int layer_;
};

