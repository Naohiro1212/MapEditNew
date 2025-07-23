#include "Stage.h"
#include "Input.h"

Info infos[] =
{
	{800, 200, "左クリック"},
	{820, 230, "エディター上：持っているチップを配置"},
	{820, 250, "チップ上：選択したチップを所持"},
	{820, 270, "Lシフト押しながら：エディター上の選択したチップを削除"},
	{800, 310, "右クリック"},
	{820, 340, "持っているチップを削除"},
	{800, 380, "Sキー"},
	{820, 410, "セーブ"},
	{800, 450, "Lキー"},
	{820, 480, "ロード"},
	{900, 380, "Cキー"}, 
	{920, 410, "指定した層をすべてクリア"}
};

Stage::Stage()
	: GameObject()
{
	mapChip_ = new MapChip(); //マップチップのインスタンスを作成
	mapEdit_ = new MapEdit(); //マップエディタのインスタンスを作成
}

Stage::~Stage()
{
	delete mapChip_; //マップチップのインスタンスを解放
}

void Stage::Update()
{
}

void Stage::Draw()
{
	int color = GetColor(255, 255, 255);
	for (const auto& info : infos) {
		DrawString(info.x, info.y, info.text, color);
	}
}


