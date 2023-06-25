#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>

#include <Lori/Core/Serializable.h>
#include <Lori/Graphics/Colour.h>

using namespace std;

template <> string Serialize(const string& value) { return value; }

template <> string Serialize(const RGBAColour& colour)
{
    stringstream ss;
    ss << "#" << hex << RGBAColour::ToARGB(colour);

    return ss.str();
}

template <> string Deserialize(const string& value) { return value; }

template <> RGBAColour Deserialize(const string& value)
{
    if(value.front() == '#')
        return RGBAColour::FromARGB(strtoul(value.c_str() + 1, nullptr, 16));

    return RGBAColour::FromARGB(static_cast<uint32_t>(stoul(value, nullptr, 16)));
}
