#ifndef CITY_HPP
#define CITY_HPP

#include <utility>


class City
{
public:
    City() = default;
    City(int x, int y);
    [[nodiscard]] auto distance(City to) const -> double;
    [[nodiscard]] auto get_coords() const -> std::pair<int, int>;
private:
    std::pair<int, int> _coords;
};

#endif