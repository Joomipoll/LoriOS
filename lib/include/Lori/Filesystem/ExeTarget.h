#pragma once

#include <vector>
#include <string_view>

using namespace std;

namespace Lori
{
	namespace FileSystem
	{
		class ExeTarget
		{
		public:
			ExeTarget(const char* path);
    		ExeTarget(const string& path);

    		const string_view& Executable();
    		const vector<string_view>& Arguments();

  		private:
    		string_view executable;
    		vector<string_view> arguments;
		}
	}
}
