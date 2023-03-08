#include <thread>
#include <random>
#include <algorithm>
#include <fmt/format.h>
#include <spl/image.hpp>
#include <spl/primitive.hpp>

#include "population.hpp"
#include "thread_pool.hpp"

namespace sgl = spl::graphics;


int main() {
    constexpr auto image_size = 1000;
    const auto num_of_threads = std::thread::hardware_concurrency();
    constexpr auto pop_per_thread = 10000;
    constexpr auto num_of_cities = 100;
    constexpr auto max_num_of_generations = 5000;
    const auto pop_size = num_of_threads * pop_per_thread;
    constexpr auto check_every_n_gens = 250;
    constexpr auto perc_of_promotions = 0.2;
    constexpr auto mutation_ratio = 0.3;

    // init cities randomly
    auto rng = std::mt19937{std::random_device{}()};
    auto dist = std::uniform_int_distribution<int>(0, image_size);
    auto city_list = std::vector<City>{};
    city_list.reserve(num_of_cities);
    for ( auto i = 0; i < num_of_cities; ++i ) {
        auto x = dist(rng);
        auto y = dist(rng);
        city_list.emplace_back(x, y);
    }

    // init pop with random genomes
    auto initial_genomes = std::vector<Genome>{};
    initial_genomes.reserve(pop_size);
    for ( auto i = 0u; i < pop_size; ++i ) {
        std::ranges::shuffle(city_list, rng);

        const auto genome = city_list;
        initial_genomes.emplace_back(genome);
    }
    auto population = Population(initial_genomes);
    auto best_of_generation = std::vector<Genome>{};
    best_of_generation.reserve(max_num_of_generations);

    // evolve
    auto best_for_now = 0.;
    for ( auto gen = 0; gen < max_num_of_generations; ++gen ) {
        population.evaluate();
        best_of_generation.emplace_back(population.get_best());
        const auto best_of_gen = best_of_generation[gen].get_score();
        population.evolve(static_cast<int>(pop_size * perc_of_promotions), mutation_ratio);
        if ( gen % check_every_n_gens == 0 ) {
            fmt::print("gen {} done, best score is {}\n", gen, best_of_gen);
            if ( best_of_generation[gen].get_score() == best_for_now )
            {
                fmt::print("pop is now stable, skipping {} generations\n", max_num_of_generations - gen);
                break;
            }
            best_for_now = best_of_gen;
        }
    }

    // lambda used when saving to image
    auto save_states = [&](uint start, uint stop) {
        auto image = sgl::image(image_size, image_size, sgl::color::white);
        for (auto gen = start; gen < stop; ++gen) {
            const auto genes = best_of_generation[gen].get_genes();
            for (auto i = 1u; i < genes.size(); ++i) {
                const auto from = genes[i].get_coords();
                const auto to = genes[i - 1].get_coords();
                const auto a = static_cast<long>(from.first);
                const auto b = static_cast<long>(from.second);
                const auto x = static_cast<long>(to.first);
                const auto y = static_cast<long>(to.second);
                image.draw(sgl::line({a, b}, {x, y}));
            }
            const auto from = genes[genes.size() - 1].get_coords();
            const auto to = genes[0].get_coords();
            const auto a = static_cast<long>(from.first);
            const auto b = static_cast<long>(from.second);
            const auto x = static_cast<long>(to.first);
            const auto y = static_cast<long>(to.second);
            image.draw(sgl::line({a, b}, {x, y}));

            for (auto const & c: city_list) {
                auto [i, j] = c.get_coords();
                image.draw(spl::graphics::circle{{static_cast<long>(i), static_cast<long>(j)}, 2}.fill_color(
                        sgl::color::red));
            }

            image.save_to_file(fmt::format("outs/generation_{:04}.jpg", gen));
            image.fill(sgl::color::white);
        }
    };

    // we can shorten the size of this vector since the last states are all the same, so we cut half of them
    best_of_generation.erase(best_of_generation.end() - check_every_n_gens / 2, best_of_generation.end());
    const auto gen_size = best_of_generation.size();
    // multi-thread
    const auto gen_per_core = gen_size / num_of_threads;
    auto compute = [&](uint worker) {
        auto start = worker * gen_per_core;
        auto stop = start + gen_per_core;
        save_states(start, stop);
    };

    auto workers = std::vector<thread_pool::task_handler>{};

    for (auto w = 0u; w < num_of_threads; ++w) { workers.emplace_back(thread_pool.schedule(compute, w)); }
    for (auto & w : workers) { w.join(); }

    // in case we do not define the number of generations as multiple of the thead count, we need to check if
    // we skipped some generations when dividing the vector with the save-states between the different threads
    auto end = gen_per_core * num_of_threads;
    if ( end != gen_size ) {
        save_states(end, gen_size);
    }

    fmt::print("done\n");

    return 0;
}
