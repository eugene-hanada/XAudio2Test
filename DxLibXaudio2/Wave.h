#pragma once
#include <filesystem>
#include <vector>

// Wave�N���X
class Wave
{
public:

	// fmt�`�����N�̃f�[�^
	struct FMT
	{
		unsigned int size = 0u;
		unsigned short type = 0u;
		unsigned short channel = 0u;
		unsigned int sample = 0u;
		unsigned int byte = 0u;
		unsigned short block = 0u;
		unsigned short bit = 0u;
	};

	Wave();

	/// <summary>
	/// wave�t�@�C�������[�h����
	/// </summary>
	/// <param name="path"> �t�@�C���p�X </param>
	/// <returns> ������true </returns>
	bool Load(const std::filesystem::path& path);

	/// <summary>
	/// fmt�̕������擾����
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const FMT& GetFmt(void) const&
	{
		return fmt_;
	}

	/// <summary>
	/// �f�[�^���擾����
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const std::vector<unsigned char>& GetData(void) const&
	{
		return data_;
	}

private:

	/// <summary>
	/// Fmt�`�����N�̓ǂݍ���
	/// </summary>
	/// <param name="file"></param>
	void LoadFmt(std::ifstream& file);

	/// <summary>
	/// �f�[�^�`�����N�̓ǂݍ���
	/// </summary>
	/// <param name="file"></param>
	void LoadData(std::ifstream& file);


	// �f�[�^
	std::vector<unsigned char> data_;

	// fmt
	FMT fmt_;

	// RiFF�`�����N
	struct RIFF
	{
		char id[4];
		unsigned int size;
		char type[4];
	};

};

