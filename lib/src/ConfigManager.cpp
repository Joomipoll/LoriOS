#include <functional>

#include <Lori/Core/JSON.h>
#include <Lori/Core/Logger.h>
#include <Lori/Core/ConfigManager.h>

using namespace std;
using namespace Logger;

namespace Lori
{
    void ConfigManager::LoadJSONConfig(const string& path)
    {
        JSONParser parser(path.c_str());

        function<void(const string&, JSONValue&)> readObject;
        readObject = [this, &readObject](const string& configPrefix, JSONValue& object) -> void {
            assert(object.IsObject());
            auto& obj = *object.data.object;
            for (auto& val : obj)
            {
                if (val.second.IsObject())
                {
                    readObject(configPrefix + val.first + ".", val.second);
                } else if(auto it = m_entries.find(configPrefix + val.first); it != m_entries.end()) {
                    ConfigValue& configEntry = it->second;
                    if(holds_alternative<long>(configEntry))
                        configEntry = val.second.AsSignedNumber();
                    else if(holds_alternative<unsigned long>(configEntry))
                        configEntry = val.second.AsUnsignedNumber();
                    else if(holds_alternative<bool>(configEntry))
                        configEntry = val.second.AsBool();
                    else if(holds_alternative<string>(configEntry))
                        configEntry = val.second.AsString();
                }
            }
        };

        auto root = parser.Parse();
        if(!root.IsObject()){
            Warning("[ConfigManager] Failed to load JSON config at {}", path);
            return;
        }

        readObject("", root);
    }
}
