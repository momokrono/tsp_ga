#include "population.hpp"

#include <mutex>
#include <random>
#include <thread>
#include <algorithm>


Population::Population(std::vector<Genome> pop) : _pop{std::move(pop)} { }


auto Population::get_best() const -> Genome { return _pop[0]; }


auto Population::evaluate() -> void
{

    const auto num_of_cores = std::thread::hardware_concurrency();
    const auto pop_per_core = _pop.size() / num_of_cores;

    auto compute = [&](uint worker) -> void {
        auto start = worker * pop_per_core;
        auto stop = start + pop_per_core;
        for ( auto i = start; i < stop; ++i ) {
            _pop[i].evaluate();
        }
    };

    auto workers = std::vector<std::thread>{};

    for (auto w = 0u; w < num_of_cores; ++w) { workers.emplace_back(compute, w); }
    for (auto & w : workers) { w.join(); }

    auto end = pop_per_core * num_of_cores;
    if ( end != _pop.size() ) {
        for ( auto i = end; i < _pop.size(); ++i ) {
            _pop[i].evaluate();
        }
    }

    std::sort(_pop.begin(), _pop.end(),
              [] (Genome const & first, Genome const & second) { return first.get_score() > second.get_score(); });
}


auto Population::evolve(int const num_to_promote, double mutation_ratio) -> void {
    const auto pop_size = _pop.size();
    const auto genome_size = _pop[0].get_genes().size();

    auto new_generation = std::vector<Genome>{_pop.begin(), _pop.begin() + num_to_promote};
    new_generation.reserve(pop_size);

    std::mutex mutex;

    auto generate_pop = [&](uint start, uint stop) {
        auto rng = std::mt19937{std::random_device{}()};
        auto mutation_chance = std::uniform_real_distribution<double>(0., 1.);
        auto gene_dist = std::uniform_int_distribution<int>(0, static_cast<int>(genome_size) - 1);
        auto parent_dist = std::uniform_int_distribution<int>(0, num_to_promote - 1);
        for (auto t = start; t < stop; ++t) {
            auto genes = std::vector<City>{};
            genes.reserve(genome_size);

            const auto p1 = parent_dist(rng);
            const auto p2 = parent_dist(rng);
            const auto parent_1 = new_generation[p1].get_genes();
            const auto parent_2 = new_generation[p2].get_genes();

            auto cut_point_start = gene_dist(rng);
            auto cut_point_end = gene_dist(rng);
            if (cut_point_start > cut_point_end) {
                std::swap(cut_point_start, cut_point_end);
            }
            for (auto g = 0u; g < genome_size; ++g) {
                if (g < static_cast<long>(cut_point_end) and g >= static_cast<long>(cut_point_start)) {
                    genes.push_back(parent_1[g]);
                }
            }
            for (auto const &g: parent_2) {
                const auto coords = g.get_coords();
                const auto find = std::find_if(genes.begin(), genes.end(),
                                         [&coords](City const &c) { return coords == c.get_coords(); });
                if (find == genes.end()) {
                    genes.push_back(g);
                }
            }

            if ( mutation_chance(rng) <  mutation_ratio ) {
                const auto pick_f = gene_dist(rng);
                const auto pick_s = gene_dist(rng);
                const auto f = genes[pick_f];
                const auto s = genes[pick_s];
                genes[pick_f] = s;
                genes[pick_s] = f;
            }

            std::scoped_lock lock{mutex};
            new_generation.emplace_back(genes);
        }
    };

    const auto num_of_threads = std::thread::hardware_concurrency();
    const auto pop_per_thread = (pop_size - num_to_promote) / num_of_threads;

    auto compute = [&](uint worker) {
        const auto start = worker * pop_per_thread;
        const auto stop = start + pop_per_thread;
        generate_pop(start, stop);
    };

    auto workers = std::vector<std::thread>{};

    for (auto w = 0u; w < num_of_threads; ++w) { workers.emplace_back(compute, w); }
    for (auto & w : workers) { w.join(); }

    auto end = pop_per_thread * num_of_threads;
    if ( end != pop_size - num_to_promote ) {
        generate_pop(end, pop_size - num_to_promote);
    }

    _pop = new_generation;
}