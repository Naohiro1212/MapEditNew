#include <windows.h>
#include "MapEdit.h"
#include <cassert>
#include "Input.h"
#include "DxLib.h"
#include "MapChip.h"
#include <fstream>
#include <codecvt>
#include <sstream>
#include <string>
#include <iostream>


MapEdit::MapEdit()
	:GameObject(), efg_(GetMapEditConfig()),layer_(0), //初期値を-1で20*20の配列を初期化する
	isInMapEditArea_(false) //マップエディタ領域内にいるかどうか
{
	myMap_.resize(3);
	for (auto& row : myMap_)
	{
		row.resize(efg_.MAP_HEIGHT * efg_.MAP_WIDTH, -1);
	}

	mapEditRect_ = { efg_.LEFT_MARGIN, efg_.TOP_MARGIN,
		efg_.MAP_WIDTH * efg_.MAP_IMAGE_SIZE, efg_.MAP_HEIGHT * efg_.MAP_IMAGE_SIZE};
}

MapEdit::~MapEdit()
{
}

void MapEdit::SetMap(Point p, int value, int nowrow)
{
	//マップの座標pにvalueをセットする
	//pが、配列の範囲外の時はassertにひっかかる
	assert(p.x >= 0 && p.x < efg_.MAP_WIDTH);
	assert(p.y >= 0 && p.y < efg_.MAP_HEIGHT);
	myMap_[nowrow][p.y * efg_.MAP_WIDTH + p.x] = value; //y行x列にvalueをセットする

}

int MapEdit::GetMap(Point p, int nowrow) const
{
	//マップの座標pの値を取得する
	//pが、配列の範囲外の時はassertにひっかかる
	assert(p.x >= 0 && p.x < efg_.MAP_WIDTH);
	assert(p.y >= 0 && p.y < efg_.MAP_HEIGHT);
	return myMap_[nowrow][p.y * efg_.MAP_WIDTH + p.x]; //y行x列の値を取得する

}

void MapEdit::Update()
{
	Point mousePos;
	if (GetMousePoint(&mousePos.x, &mousePos.y) == -1) {
		return;
	}
	// マウスの座標がマップエディタ領域内にいるかどうかを判定する
	isInMapEditArea_ = mousePos.x > mapEditRect_.x && mousePos.x < mapEditRect_.x + mapEditRect_.w &&
		mousePos.y > mapEditRect_.y && mousePos.y < mapEditRect_.y + mapEditRect_.h;

	//左上　mapEditRect_.x, mapEditRect_.y
	//右上　mapEditRect_.x + mapEditRect_.w, mapEditRect_.y
	//右下  mapEditRect_.x + mapEditRect_.w, mapEditRect_.y + mapEditRect_.h
	//左下  mapEditRect_.x, mapEditRect_.y + mapEditRect_.h
		// グリッド座標に変換
	if (!isInMapEditArea_) {
		return; //マップエディタ領域外なら何もしない
	}

	int gridX = (mousePos.x - efg_.LEFT_MARGIN) / efg_.MAP_IMAGE_SIZE;
	int gridY = (mousePos.y - efg_.TOP_MARGIN) / efg_.MAP_IMAGE_SIZE;

	drawAreaRect_ = { efg_.LEFT_MARGIN + gridX * efg_.MAP_IMAGE_SIZE,
		efg_.TOP_MARGIN + gridY * efg_.MAP_IMAGE_SIZE,
		efg_.MAP_IMAGE_SIZE, efg_.MAP_IMAGE_SIZE };

	//マウスのボタンが押されたら、持ってる画像をその座標に貼る
	//if (Input::IsButtonDown(MOUSE_INPUT_LEFT)) //左クリックでマップに値をセット
	//{
	//	MapChip* mapChip = FindGameObject<MapChip>();

	//	if (CheckHitKey(KEY_INPUT_LSHIFT)) //Rキーを押しているなら
	//	{
	//		SetMap({ gridX, gridY }, -1); //マップに値をセット（-1は何もない状態）
	//		return; //マップチップを削除したらここで終了
	//	}
	//	else if (mapChip && mapChip->IsHold()) //マップチップを持っているなら
	//	{
	//		SetMap({ gridX, gridY }, mapChip->GetHoldImage()); //マップに値をセット
	//	}
	//}

	if (Input::IsButtonKeep(MOUSE_INPUT_LEFT)) //左クリックでマップに値をセット
	{
		MapChip* mapChip = FindGameObject<MapChip>();

		if (CheckHitKey(KEY_INPUT_LSHIFT)) //Rキーを押しているなら
		{
			printfDx("マスが消されています\n");
			SetMap({ gridX, gridY }, -1, layer_); //マップに値をセット（-1は何もない状態）
			return; //マップチップを削除したらここで終了
		}
		else if (mapChip && mapChip->IsHold()) //マップチップを持っているなら
		{
			printfDx("チップが設置されています\n");
			SetMap({ gridX, gridY }, mapChip->GetHoldImage(), layer_); //マップに値をセット
		}
	}
	if (Input::IsKeyDown(KEY_INPUT_S))
	{
		SaveMapData();
	}
	if (Input::IsKeyDown(KEY_INPUT_L))
	{
		LoadMapData();
	}
	if (Input::IsKeyDown(KEY_INPUT_C))
	{
		printfDx("クリアされました\n");
		ClearLayer();
	}

	ChangeLayer();
}

void MapEdit::Draw()
{//背景を描画する

	for (int layer = 0;layer < 3;layer++)
	{
		for (int j = 0;j < efg_.MAP_HEIGHT;j++)
		{
			for (int i = 0; i < efg_.MAP_WIDTH; i++)
			{
				int value = GetMap({ i,j }, layer);
				if (value != -1) //-1なら何も描画しない
				{
					DrawGraph(efg_.LEFT_MARGIN + i * efg_.MAP_IMAGE_SIZE,
						efg_.TOP_MARGIN + j * efg_.MAP_IMAGE_SIZE,
						value, TRUE);
				}
			}
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(efg_.LEFT_MARGIN + 0, efg_.TOP_MARGIN + 0,
		efg_.LEFT_MARGIN + efg_.MAP_WIDTH * efg_.MAP_IMAGE_SIZE, 
		efg_.TOP_MARGIN + efg_.MAP_HEIGHT * efg_.MAP_IMAGE_SIZE,
		GetColor(255, 255, 0), FALSE, 5);
	for (int j = 0; j < efg_.MAP_HEIGHT; j++) {
		for (int i = 0; i < efg_.MAP_WIDTH; i++) {
			DrawLine(efg_.LEFT_MARGIN + i * efg_.MAP_IMAGE_SIZE, efg_.TOP_MARGIN + j * efg_.MAP_IMAGE_SIZE,
				efg_.LEFT_MARGIN + (i + 1) * efg_.MAP_IMAGE_SIZE, efg_.TOP_MARGIN + j * efg_.MAP_IMAGE_SIZE,
				GetColor(255, 255, 255), 1);
			DrawLine(efg_.LEFT_MARGIN + i * efg_.MAP_IMAGE_SIZE, efg_.TOP_MARGIN + j * efg_.MAP_IMAGE_SIZE,
				efg_.LEFT_MARGIN + i * efg_.MAP_IMAGE_SIZE, efg_.TOP_MARGIN + (j + 1) * efg_.MAP_IMAGE_SIZE, 
				GetColor(255, 255, 255), 1);
		}
	}
	if (isInMapEditArea_) {

		DrawBox(drawAreaRect_.x, drawAreaRect_.y,
			drawAreaRect_.x + drawAreaRect_.w, drawAreaRect_.y + drawAreaRect_.h,
			GetColor(255, 255, 0), TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	for (int i = 0;i < 3;i++)
	{

		int displayIndex = 2 - i; // 逆順にして表示位置を反転
		if (layer_ == displayIndex)
		{
			DrawBox(800, 540 + 30 * i, 830, 540 + 30 * i + 30, GetColor(255, 255, 255), FALSE);
			DrawBox(801, 541 + 30 * i, 829, 540 + 30 * i + 29, GetColor(0,255,0), TRUE);
		}
		else
		{
			DrawBox(800, 540 + 30 * i, 830, 540 + 30 * i + 30, GetColor(255, 255, 255), FALSE);
		}
	}
}

void MapEdit::SaveMapData()
{
	//頑張ってファイル選択ダイアログを出す回
	TCHAR filename[255] = "";
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(ofn);
	//ウィンドウのオーナー＝親ウィンドウのハンドル
	ofn.hwndOwner = GetMainWindowHandle();
	ofn.lpstrFilter = "全てのファイル (*.*)\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = 255;
	ofn.Flags = OFN_OVERWRITEPROMPT;


	if (GetSaveFileName(&ofn))
	{
		printfDx("ファイルが選択された\n");
		//ファイルを開いて、セーブ
		//std::filesystem ファイル名だけ取り出す
		//ofstreamを開く
		std::ofstream openfile(filename);
		//ファイルの選択がキャンセル
		printfDx("セーブがキャンセル\n");
		openfile << "#TinyMapData\n";

		MapChip* mc = FindGameObject<MapChip>();

		for (int layer = 0;layer < 3;layer++)
		{
			openfile << "#Layer " << layer << "\n";
			for (int j = 0; j < efg_.MAP_HEIGHT; j++) {
				for (int i = 0; i < efg_.MAP_WIDTH; i++) {

					int index;
					if (myMap_[layer][j * efg_.MAP_WIDTH + i] != -1)
						index = mc->GetChipIndex(myMap_[layer][j * efg_.MAP_WIDTH + i]);
					else
						index = -1;

					if (i == efg_.MAP_WIDTH - 1) //最後の要素なら改行しない
					{
						openfile << index; //最後の要素はカンマをつけない
					}
					else
					{
						//最後の要素以外はカンマをつける
						openfile << index << ",";
					}
				}
				openfile << std::endl;
			}
		}
		openfile.close();
		printfDx("File Saved!!!\n");
	}
}

void MapEdit::LoadMapData()
{
	TCHAR filename[255] = "";
	OPENFILENAME ifn = { 0 };

	ifn.lStructSize = sizeof(ifn);
	ifn.hwndOwner = GetMainWindowHandle();
	ifn.lpstrFilter = "全てのファイル (*.*)\0*.*\0";
	ifn.lpstrFile = filename;
	ifn.nMaxFile = 255;

	if (GetOpenFileName(&ifn))
	{
		printfDx("ファイルが選択された→%s\n", filename);

		std::ifstream inputfile(filename);
		if (!inputfile)
		{
			printfDx("ファイルを開けませんでした\n");
			return;
		}

		MapChip* mc = FindGameObject<MapChip>();
		myMap_.clear();
		myMap_.resize(3); // 3層分用意

		int currentLayer = -1;

		std::string line;
		while (std::getline(inputfile, line)) {
			if (line.empty()) continue;

			if (line[0] == '#') {
				// #Layer 0 のようなヘッダーを検出
				if (line.find("Layer") != std::string::npos)
				{
					// 例: "#Layer 0"
					size_t pos = line.find("Layer");
					if (pos != std::string::npos)
					{
						try {
							currentLayer = std::stoi(line.substr(pos + 6)); // "Layer "は6文字
						}
						catch (...) {
							currentLayer = -1;
						}

						if (currentLayer < 0 || currentLayer >= 3) {
							printfDx("Layer番号がおかしい\n");
							currentLayer = -1; //無効にする
						}
					}
				}
				continue; // ヘッダー行は読み込みデータ無し
			}

			if (currentLayer == -1) {
				// レイヤー不明なら無視
				continue;
			}

			// レイヤーに対応した行を読み込み
			std::istringstream iss(line);
			std::string tmp;

			while (std::getline(iss, tmp, ',')) {
				if (tmp == "-1") {
					myMap_[currentLayer].push_back(-1);
				}
				else {
					int index = std::stoi(tmp);
					int handle = mc->GetHandle(index);
					myMap_[currentLayer].push_back(handle);
				}
			}
		}
		// 読み込み完了後に念のため、各層のサイズが正しいかチェックやresizeも検討
		for (int i = 0; i < 3; ++i)
		{
			if (myMap_[i].size() != efg_.MAP_HEIGHT * efg_.MAP_WIDTH)
			{
				printfDx("Layer %d のサイズが異常です。正規化します。\n", i);
				myMap_[i].resize(efg_.MAP_HEIGHT * efg_.MAP_WIDTH, -1);
			}
		}
		printfDx("マップデータの読み込み完了\n");
	}
	else
	{
		printfDx("ファイルの選択がキャンセルされました\n");
	}
}


void MapEdit::ChangeLayer()
{
	// 仮レイヤーチェンジ
	// todo マジックナンバーを消す
	if (Input::IsKeyDown(KEY_INPUT_D)) // Dキーで下の層へ
	{
		if (layer_ > 0)
		{
			layer_--;
		}
	}
	if (Input::IsKeyDown(KEY_INPUT_U)) // Uキーで上の層へ
	{
		if (layer_ < 2)
		{
			layer_++;
		}
	}
}

void MapEdit::ClearLayer()
{
	std::fill(myMap_[layer_].begin(), myMap_[layer_].end(), -1);
}


