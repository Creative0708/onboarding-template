#include "grid.hpp"
#include <iostream>
#include <chrono>

void apply_stencil(Grid& new_grid, const Grid& old_grid) {
    auto& new_cell{new_grid.cell()};
    const auto& old_cell{old_grid.cell()};

    // Copy boundaries
    for (std::size_t i{}; i < old_grid.Nx(); ++i) {
        new_cell[i][0] = old_cell[i][0];
        new_cell[i][old_grid.Ny() - 1] = old_cell[i][old_grid.Ny() - 1];
    }

    for (std::size_t j{}; j < old_grid.Ny(); ++j) {
        new_cell[0][j] = old_cell[0][j];
        new_cell[old_grid.Nx() - 1][j] = old_cell[old_grid.Nx() - 1][j];
    }

    for (std::size_t i{1}; i < new_grid.Nx() - 1; ++i) {
        for (std::size_t j{1}; j < new_grid.Ny() - 1; ++j) {
            new_cell[i][j] = 0.5 * old_cell[i][j] + 
                             0.125 * (
                                      old_cell[i-1][j] +
                                      old_cell[i+1][j] +
                                      old_cell[i][j-1] +
                                      old_cell[i][j+1]
                                     );
        }
    }
}

int main() {
    // Grid Size:
    std::size_t Nx{1024}, Ny{1024};
    std::cout << "Enter X cells: ";
    std::cin >> Nx; std::cout << std::endl;
    std::cout << "Enter Y cells: ";
    std::cin >> Ny; std::cout << std::endl;

    // Initialize grids:
    Grid old_grid{Nx, Ny};
    Grid new_grid{Nx, Ny};

    // Run for steps:
    std::size_t num_steps{1000};
    std::cout << "Enter total steps: ";
    std::cin >> num_steps; std::cout << "\n";

    // Time loop:
    auto start{std::chrono::high_resolution_clock::now()};

    // Loop update:
    for (std::size_t t{}; t < num_steps; ++t) {
        apply_stencil(new_grid, old_grid);
    }

    // Calculate total time:
    auto end{std::chrono::high_resolution_clock::now()};
    double ms{std::chrono::duration<double, std::milli>(end - start).count()};

    // Output total time:
    std::cout << "Time elapsed: " << ms << " ms" << std::endl;

    return 0;
}