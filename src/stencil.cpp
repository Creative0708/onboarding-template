#include "grid.hpp"

// Apply the five-point stencil over all interior points, copying the boundary
// values unchanged from old_grid to new_grid.
//
// This naive version is correct but unoptimized -- improving it (together with
// the Grid memory layout in grid.hpp) is the exercise.
void apply_stencil(const Grid& old_grid, Grid& new_grid) {
    const std::size_t Nx = old_grid.Nx();
    const std::size_t Ny = old_grid.Ny();

    // Copy boundaries.
    for (std::size_t i = 0; i < Nx; ++i) {
        new_grid(i, 0) = old_grid(i, 0);
        new_grid(i, Ny - 1) = old_grid(i, Ny - 1);
    }
    for (std::size_t j = 0; j < Ny; ++j) {
        new_grid(0, j) = old_grid(0, j);
        new_grid(Nx - 1, j) = old_grid(Nx - 1, j);
    }

    // Interior update.
    for (std::size_t i = 1; i < Nx - 1; ++i) {
        for (std::size_t j = 1; j < Ny - 1; ++j) {
            new_grid(i, j) = 0.5 * old_grid(i, j) +
                             0.125 * (old_grid(i - 1, j) +
                                      old_grid(i + 1, j) +
                                      old_grid(i, j - 1) +
                                      old_grid(i, j + 1));
        }
    }
}
