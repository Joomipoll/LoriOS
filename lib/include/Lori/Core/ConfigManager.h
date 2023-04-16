#pragma once

#include <concepts>
#include <cassert>
#include <map>
#include <variant>
#include <type_traits>

#include <Lori/Core/JSON.h>
#include <Lori/Core/Serializable.h>

using namespace std;

namespace Lori
{
	template <typename T>
	concept ConfigValueType = (same_as<T, string> || same_as<T, bool> || same_as<T, long> || same_as<T, unsigned long> || same_as<T, double>);

	using ConfigValue = variant<string, bool, long, unsigned long, double>;

	class ConfigManager
	{
	public:
    	ConfigManager() = default;
    	~ConfigManager() = default;

    	template <Serializable T> inline void AddSerializedConfigProperty(const string& name, const T& defaultValue)
		{
        	AddConfigProperty<string>(name, Serialize<T>(defaultValue));
    	}

    	template <ConfigValueType T> void AddConfigProperty(const string& name, T defaultValue)
		{
        	if constexpr (is_trivial<T>())
            	m_entries[name] = defaultValue;
        	else
            	m_entries[name] = move(defaultValue);
    	}

    	template <ConfigValueType T> inline T GetConfigProperty(const string& name)
		{
        	auto& v = m_entries.at(name);
        	assert(holds_alternative<T>(v));

        	return get<T>(v);
    	}

    	template <Serializable T> inline T GetSerializedConfigProperty(const string& name)
		{
        	return Deserialize<T>(get<string>(m_entries.at(name)));
    	}

    	void LoadJSONConfig(const string& path);

	private:
    	map<string, ConfigValue> m_entries;
	};
}
