#ifndef CEF_TESTS_SHARED_COMMON_CLIENT_UTIILS_H_
#define CEF_TESTS_SHARED_COMMON_CLIENT_UTIILS_H_
#pragma once
#include <vector>
 
#include "include/cef_app.h"
namespace client {
	class ClientUtils {
	public:
		ClientUtils();
		static std::vector<std::string> split(std::string str, std::string pattern);
		 
	};
}

#endif 