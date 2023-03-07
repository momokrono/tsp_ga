#include "city.hpp"
#include <cmath>

City::City(int x, int y): _coords{x, y} { }

auto City::distance(City to) const -> double
{
    auto [xt, yt] = to.get_coords();
    auto [xf, yf] = _coords;
    xf -= xt;
    yf -= yt;
    return xf * xf + yf * yf ;
}

auto City::get_coords() const -> std::pair<int, int>
{
    return _coords;
}
