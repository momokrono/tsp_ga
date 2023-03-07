#include "genome.hpp"
#include "fmt/core.h"


Genome::Genome(std::vector<City> genes) : _genes{std::move(genes)}, _score{0.} { }


auto Genome::get_genes() const -> std::vector<City> { return _genes; }


auto Genome::get_score() const -> double { return _score; }


auto Genome::evaluate() -> void
{
   for ( auto i = 1u; i < _genes.size(); ++i) {
        _score += _genes[i].distance(_genes[i - 1]);
    }
    _score += _genes[_genes.size() - 1].distance(_genes[0]);
    _score = 1. / _score;
}
