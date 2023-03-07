#ifndef POPULATION_HPP
#define POPULATION_HPP

#include "genome.hpp"


class Population
{
public:
    Population() = default;
    explicit Population(std::vector<Genome> pop);

    auto evaluate() -> void;
    [[nodiscard]] auto get_best() const -> Genome;
    auto evolve(int num_to_promote, double mutation_ratio) -> void;

private:
    std::vector<Genome> _pop;
};


#endif