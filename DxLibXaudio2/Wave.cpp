#include "Wave.h"
#include <fstream>


Wave::Wave():
	fmt_{}
{
}

bool Wave::Load(const std::filesystem::path& path)
{
	std::ifstream file{ path , std::ios::binary};

	if (!file)
	{
		return false;
	}

	RIFF riff;
	file.read(reinterpret_cast<char*>(&riff), sizeof(riff));

	std::string id;
	id.resize(4);
	std::string fmtStr{ 'f','m','t',' ' };

	while (true)
	{
		file.read(id.data(), 4);
		if (file.eof())
		{
			return true;
		}

		if (id == fmtStr)
		{
			LoadFmt(file);
		}
		else
		{
			LoadData(file);
		}
	}

	return true;
}

void Wave::LoadFmt(std::ifstream& file)
{
	file.read(reinterpret_cast<char*>(&fmt_), sizeof(fmt_));
}

void Wave::LoadData(std::ifstream& file)
{
	std::uint32_t size{ 0 };
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	data_.resize(size);
	file.read(reinterpret_cast<char*>(data_.data()), size);
}
