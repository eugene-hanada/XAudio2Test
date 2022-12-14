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

	char id[4];
	file.read(id, 4);
	file.read(reinterpret_cast<char*>(&fmt_), sizeof(fmt_));

	DataHeader dh;
	file.read(reinterpret_cast<char*>(&dh), sizeof(dh));

	data_.resize(dh.size);
	file.read(reinterpret_cast<char*>(data_.data()), dh.size);


	return true;
}
