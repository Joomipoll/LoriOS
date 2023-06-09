#pragma once

typedef struct Vector2i
{
    int x, y;
} vector2i_t;

inline vector2i_t operator+(const vector2i_t& l, const vector2i_t& r)
{
    return {l.x + r.x, l.y + r.y};
}
inline vector2i_t operator-(const vector2i_t& l, const vector2i_t& r)
{
    return { l.x - r.x, l.y - r.y };
}

inline bool operator==(const vector2i_t& l, const vector2i_t& r)
{
    return l.x == r.x && l.y == r.y;
}
inline bool operator!=(const vector2i_t& l, const vector2i_t& r)
{
    return l.x != r.x && l.y != r.y;
}

inline void operator+=(vector2i_t& l, const vector2i_t& r){ l = l + r; }
inline void operator-=(vector2i_t& l, const vector2i_t& r){ l = l - r; }
