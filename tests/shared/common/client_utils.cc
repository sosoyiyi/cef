#include "tests/shared/common/client_utils.h"
namespace client {

	ClientUtils::ClientUtils() {
	}

	 
	//static 
	std::vector<std::string> ClientUtils::split(std::string str, std::string pattern)
	{
		std::vector<std::string> ret;
		if (pattern.empty()) return ret;
		size_t start = 0, index = str.find_first_of(pattern, 0);
		while (index != str.npos)
		{
			if (start != index)
				ret.push_back(str.substr(start, index - start));
			start = index + 1;
			index = str.find_first_of(pattern, start);
		}
		if (!str.substr(start).empty())
			ret.push_back(str.substr(start));
		return ret;
	}

}