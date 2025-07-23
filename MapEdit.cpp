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
	:GameObject(), efg_(GetMapEditConfig()),layer_(0), //�����l��-1��20*20�̔z�������������
	isInMapEditArea_(false) //�}�b�v�G�f�B�^�̈���ɂ��邩�ǂ���
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
	//�}�b�v�̍��Wp��value���Z�b�g����
	//p���A�z��͈̔͊O�̎���assert�ɂЂ�������
	assert(p.x >= 0 && p.x < efg_.MAP_WIDTH);
	assert(p.y >= 0 && p.y < efg_.MAP_HEIGHT);
	myMap_[nowrow][p.y * efg_.MAP_WIDTH + p.x] = value; //y�sx���value���Z�b�g����

}

int MapEdit::GetMap(Point p, int nowrow) const
{
	//�}�b�v�̍��Wp�̒l���擾����
	//p���A�z��͈̔͊O�̎���assert�ɂЂ�������
	assert(p.x >= 0 && p.x < efg_.MAP_WIDTH);
	assert(p.y >= 0 && p.y < efg_.MAP_HEIGHT);
	return myMap_[nowrow][p.y * efg_.MAP_WIDTH + p.x]; //y�sx��̒l���擾����

}

void MapEdit::Update()
{
	Point mousePos;
	if (GetMousePoint(&mousePos.x, &mousePos.y) == -1) {
		return;
	}
	// �}�E�X�̍��W���}�b�v�G�f�B�^�̈���ɂ��邩�ǂ����𔻒肷��
	isInMapEditArea_ = mousePos.x > mapEditRect_.x && mousePos.x < mapEditRect_.x + mapEditRect_.w &&
		mousePos.y > mapEditRect_.y && mousePos.y < mapEditRect_.y + mapEditRect_.h;

	//����@mapEditRect_.x, mapEditRect_.y
	//�E��@mapEditRect_.x + mapEditRect_.w, mapEditRect_.y
	//�E��  mapEditRect_.x + mapEditRect_.w, mapEditRect_.y + mapEditRect_.h
	//����  mapEditRect_.x, mapEditRect_.y + mapEditRect_.h
		// �O���b�h���W�ɕϊ�
	if (!isInMapEditArea_) {
		return; //�}�b�v�G�f�B�^�̈�O�Ȃ牽�����Ȃ�
	}

	int gridX = (mousePos.x - efg_.LEFT_MARGIN) / efg_.MAP_IMAGE_SIZE;
	int gridY = (mousePos.y - efg_.TOP_MARGIN) / efg_.MAP_IMAGE_SIZE;

	drawAreaRect_ = { efg_.LEFT_MARGIN + gridX * efg_.MAP_IMAGE_SIZE,
		efg_.TOP_MARGIN + gridY * efg_.MAP_IMAGE_SIZE,
		efg_.MAP_IMAGE_SIZE, efg_.MAP_IMAGE_SIZE };

	//�}�E�X�̃{�^���������ꂽ��A�����Ă�摜�����̍��W�ɓ\��
	//if (Input::IsButtonDown(MOUSE_INPUT_LEFT)) //���N���b�N�Ń}�b�v�ɒl���Z�b�g
	//{
	//	MapChip* mapChip = FindGameObject<MapChip>();

	//	if (CheckHitKey(KEY_INPUT_LSHIFT)) //R�L�[�������Ă���Ȃ�
	//	{
	//		SetMap({ gridX, gridY }, -1); //�}�b�v�ɒl���Z�b�g�i-1�͉����Ȃ���ԁj
	//		return; //�}�b�v�`�b�v���폜�����炱���ŏI��
	//	}
	//	else if (mapChip && mapChip->IsHold()) //�}�b�v�`�b�v�������Ă���Ȃ�
	//	{
	//		SetMap({ gridX, gridY }, mapChip->GetHoldImage()); //�}�b�v�ɒl���Z�b�g
	//	}
	//}

	if (Input::IsButtonKeep(MOUSE_INPUT_LEFT)) //���N���b�N�Ń}�b�v�ɒl���Z�b�g
	{
		MapChip* mapChip = FindGameObject<MapChip>();

		if (CheckHitKey(KEY_INPUT_LSHIFT)) //R�L�[�������Ă���Ȃ�
		{
			printfDx("�}�X��������Ă��܂�\n");
			SetMap({ gridX, gridY }, -1, layer_); //�}�b�v�ɒl���Z�b�g�i-1�͉����Ȃ���ԁj
			return; //�}�b�v�`�b�v���폜�����炱���ŏI��
		}
		else if (mapChip && mapChip->IsHold()) //�}�b�v�`�b�v�������Ă���Ȃ�
		{
			printfDx("�`�b�v���ݒu����Ă��܂�\n");
			SetMap({ gridX, gridY }, mapChip->GetHoldImage(), layer_); //�}�b�v�ɒl���Z�b�g
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
		printfDx("�N���A����܂���\n");
		ClearLayer();
	}

	ChangeLayer();
}

void MapEdit::Draw()
{//�w�i��`�悷��

	for (int layer = 0;layer < 3;layer++)
	{
		for (int j = 0;j < efg_.MAP_HEIGHT;j++)
		{
			for (int i = 0; i < efg_.MAP_WIDTH; i++)
			{
				int value = GetMap({ i,j }, layer);
				if (value != -1) //-1�Ȃ牽���`�悵�Ȃ�
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

		int displayIndex = 2 - i; // �t���ɂ��ĕ\���ʒu�𔽓]
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
	//�撣���ăt�@�C���I���_�C�A���O���o����
	TCHAR filename[255] = "";
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(ofn);
	//�E�B���h�E�̃I�[�i�[���e�E�B���h�E�̃n���h��
	ofn.hwndOwner = GetMainWindowHandle();
	ofn.lpstrFilter = "�S�Ẵt�@�C�� (*.*)\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = 255;
	ofn.Flags = OFN_OVERWRITEPROMPT;


	if (GetSaveFileName(&ofn))
	{
		printfDx("�t�@�C�����I�����ꂽ\n");
		//�t�@�C�����J���āA�Z�[�u
		//std::filesystem �t�@�C�����������o��
		//ofstream���J��
		std::ofstream openfile(filename);
		//�t�@�C���̑I�����L�����Z��
		printfDx("�Z�[�u���L�����Z��\n");
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

					if (i == efg_.MAP_WIDTH - 1) //�Ō�̗v�f�Ȃ���s���Ȃ�
					{
						openfile << index; //�Ō�̗v�f�̓J���}�����Ȃ�
					}
					else
					{
						//�Ō�̗v�f�ȊO�̓J���}������
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
	ifn.lpstrFilter = "�S�Ẵt�@�C�� (*.*)\0*.*\0";
	ifn.lpstrFile = filename;
	ifn.nMaxFile = 255;

	if (GetOpenFileName(&ifn))
	{
		printfDx("�t�@�C�����I�����ꂽ��%s\n", filename);

		std::ifstream inputfile(filename);
		if (!inputfile)
		{
			printfDx("�t�@�C�����J���܂���ł���\n");
			return;
		}

		MapChip* mc = FindGameObject<MapChip>();
		myMap_.clear();
		myMap_.resize(3); // 3�w���p��

		int currentLayer = -1;

		std::string line;
		while (std::getline(inputfile, line)) {
			if (line.empty()) continue;

			if (line[0] == '#') {
				// #Layer 0 �̂悤�ȃw�b�_�[�����o
				if (line.find("Layer") != std::string::npos)
				{
					// ��: "#Layer 0"
					size_t pos = line.find("Layer");
					if (pos != std::string::npos)
					{
						try {
							currentLayer = std::stoi(line.substr(pos + 6)); // "Layer "��6����
						}
						catch (...) {
							currentLayer = -1;
						}

						if (currentLayer < 0 || currentLayer >= 3) {
							printfDx("Layer�ԍ�����������\n");
							currentLayer = -1; //�����ɂ���
						}
					}
				}
				continue; // �w�b�_�[�s�͓ǂݍ��݃f�[�^����
			}

			if (currentLayer == -1) {
				// ���C���[�s���Ȃ疳��
				continue;
			}

			// ���C���[�ɑΉ������s��ǂݍ���
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
		// �ǂݍ��݊�����ɔO�̂��߁A�e�w�̃T�C�Y�����������`�F�b�N��resize������
		for (int i = 0; i < 3; ++i)
		{
			if (myMap_[i].size() != efg_.MAP_HEIGHT * efg_.MAP_WIDTH)
			{
				printfDx("Layer %d �̃T�C�Y���ُ�ł��B���K�����܂��B\n", i);
				myMap_[i].resize(efg_.MAP_HEIGHT * efg_.MAP_WIDTH, -1);
			}
		}
		printfDx("�}�b�v�f�[�^�̓ǂݍ��݊���\n");
	}
	else
	{
		printfDx("�t�@�C���̑I�����L�����Z������܂���\n");
	}
}


void MapEdit::ChangeLayer()
{
	// �����C���[�`�F���W
	// todo �}�W�b�N�i���o�[������
	if (Input::IsKeyDown(KEY_INPUT_D)) // D�L�[�ŉ��̑w��
	{
		if (layer_ > 0)
		{
			layer_--;
		}
	}
	if (Input::IsKeyDown(KEY_INPUT_U)) // U�L�[�ŏ�̑w��
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


