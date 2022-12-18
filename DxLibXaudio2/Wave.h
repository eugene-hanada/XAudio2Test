#pragma once
#include <filesystem>
#include <vector>

// Waveクラス
class Wave
{
public:

	// fmtチャンクのデータ
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
	/// waveファイルをロードする
	/// </summary>
	/// <param name="path"> ファイルパス </param>
	/// <returns> 成功時true </returns>
	bool Load(const std::filesystem::path& path);

	/// <summary>
	/// fmtの部分を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const FMT& GetFmt(void) const&
	{
		return fmt_;
	}

	/// <summary>
	/// データを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const std::vector<unsigned char>& GetData(void) const&
	{
		return data_;
	}

private:

	/// <summary>
	/// Fmtチャンクの読み込み
	/// </summary>
	/// <param name="file"></param>
	void LoadFmt(std::ifstream& file);

	/// <summary>
	/// データチャンクの読み込み
	/// </summary>
	/// <param name="file"></param>
	void LoadData(std::ifstream& file);


	// データ
	std::vector<unsigned char> data_;

	// fmt
	FMT fmt_;

	// RiFFチャンク
	struct RIFF
	{
		char id[4];
		unsigned int size;
		char type[4];
	};

};

