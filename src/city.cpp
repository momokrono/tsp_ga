#include "city.hpp"
#include <cmath>

City::City(double x, double y): _coords{x, y} { }

auto City::distance(City to) const -> double
{
    auto [xt, yt] = to.get_coords();
    auto [xf, yf] = _coords;
    xf -= xt;
    yf -= yt;
    return std::sqrt(xf * xf + yf * yf);
}

auto City::get_coords() const -> std::pair<double, double>
{
    return _coords;
}
