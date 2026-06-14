#pragma once

#include <cstddef>
#include <vector>

// Starter Grid for the 2D heat-diffusion problem.
//
// You may change the internal storage however you like (that is the point of
// the exercise), but you MUST keep this public interface:
//
//     Grid(std::size_t nx, std::size_t ny);
//     std::size_t Nx() const;
//     std::size_t Ny() const;
//     double&     operator()(std::size_t i, std::size_t j);        // read/write
//     double      operator()(std::size_t i, std::size_t j) const;  // read
//
// The evaluation harness uses operator() to set initial conditions and to read
// results; it never touches your internal storage. So you are free to replace
// the vector<vector<double>> below with a flat array, blocked layout, etc.
class Grid {
private:
    std::size_t Nx_;
    std::size_t Ny_;

    // Naive layout -- this is what you are expected to improve.
    std::vector<std::vector<double>> cell_;

public:
    Grid(std::size_t nx, std::size_t ny)
        : Nx_{nx}
        , Ny_{ny}
        , cell_(nx, std::vector<double>(ny, 0.0)) {}

    std::size_t Nx() const { return Nx_; }
    std::size_t Ny() const { return Ny_; }

    double& operator()(std::size_t i, std::size_t j) { return cell_[i][j]; }
    double operator()(std::size_t i, std::size_t j) const { return cell_[i][j]; }
};
