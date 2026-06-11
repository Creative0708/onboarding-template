#pragma once

#include <cstddef>
#include <vector>

class Grid {
private:
    std::size_t Nx_;
    std::size_t Ny_;

    std::vector<std::vector<double>> cell_;

public:
    Grid(std::size_t nx, std::size_t ny)
    : Nx_{nx}
    , Ny_{ny} 
    , cell_(nx, std::vector<double>(ny, 0.0))
    { }

    std::size_t Nx() const { return Nx_; }
    std::size_t Ny() const { return Ny_; }

    std::vector<std::vector<double>>& cell() { return cell_; }
    const std::vector<std::vector<double>>& cell() const { return cell_; }
};