#pragma once

#include <string>

using namespace std;

namespace Lori
{
    class URL final
    {
    public:
        URL(const char* url);

        inline const string& Protocol() const { return protocol; }
        inline const string& UserInfo() const { return userinfo; }
        inline const string& Host() const { return host; }
        inline const string& Port() const { return port; }
        inline const string& Resource() const { return resource; }

        inline bool IsValid() const { return valid; }

    private:
        string protocol;
        string userinfo;
        string host;
        string port;
        string resource;

        bool valid = false;
    };
}
