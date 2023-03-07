#ifndef CITY_HPP
#define CITY_HPP

#include <utility>


class City
{
public:
    City() = default;
    City(double x, double y);
    [[nodiscard]] auto distance(City to) const -> double;
    [[nodiscard]] auto get_coords() const -> std::pair<double, double>;
private:
    std::pair<double, double> _coords;
};

#endif