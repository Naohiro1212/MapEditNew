#include "Stage.h"
#include "Input.h"

Info infos[] =
{
	{800, 200, "���N���b�N"},
	{820, 230, "�G�f�B�^�[��F�����Ă���`�b�v��z�u"},
	{820, 250, "�`�b�v��F�I�������`�b�v������"},
	{820, 270, "L�V�t�g�����Ȃ���F�G�f�B�^�[��̑I�������`�b�v���폜"},
	{800, 310, "�E�N���b�N"},
	{820, 340, "�����Ă���`�b�v���폜"},
	{800, 380, "S�L�["},
	{820, 410, "�Z�[�u"},
	{800, 450, "L�L�["},
	{820, 480, "���[�h"},
	{900, 380, "C�L�["}, 
	{920, 410, "�w�肵���w�����ׂăN���A"}
};

Stage::Stage()
	: GameObject()
{
	mapChip_ = new MapChip(); //�}�b�v�`�b�v�̃C���X�^���X���쐬
	mapEdit_ = new MapEdit(); //�}�b�v�G�f�B�^�̃C���X�^���X���쐬
}

Stage::~Stage()
{
	delete mapChip_; //�}�b�v�`�b�v�̃C���X�^���X�����
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


