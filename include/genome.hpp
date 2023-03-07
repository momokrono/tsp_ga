#ifndef GENOME_HPP
#define GENOME_HPP

#include <vector>
#include "city.hpp"


class Genome
{
public:
    Genome() = default;
    explicit Genome(std::vector<City> genes);
    [[nodiscard]] auto get_genes() const -> std::vector<City> const &;
    [[nodiscard]] auto get_score() const -> double;
    auto evaluate() -> void;

private:
    std::vector<City> _genes;
    double _score{};
};

#endif