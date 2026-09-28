#include "shared_types.h"

#include <chrono>
#include <cmath>
#include <stdexcept>

std::unique_ptr<Result> calculateA(std::shared_ptr<const InputData> data)
{
    if (!data || data->points.empty())
        throw std::invalid_argument("Input data is empty.");

    const auto start = std::chrono::high_resolution_clock::now();

    double value = 0.0;
    std::size_t operations = 0;

    // Поліном Лагранжа - реалізація алгоритму Студента A
    for (std::size_t i = 0; i < data->points.size(); ++i)
    {
        double term = data->points[i].y;

        for (std::size_t j = 0; j < data->points.size(); ++j)
        {
            if (i == j) continue;

            const double denominator = data->points[i].x - data->points[j].x;
            if (std::abs(denominator) < 1e-12)
                throw std::invalid_argument("Duplicate x values are not allowed.");

            term *= (data->x0 - data->points[j].x) / denominator;
            operations += 3;
        }

        value += term;
        ++operations;
    }

    const auto finish = std::chrono::high_resolution_clock::now();
    const double timeUs =
        std::chrono::duration<double, std::micro>(finish - start).count();

    return std::make_unique<Result>(
        Result{value, std::abs(value - data->referenceValue), operations, timeUs});
}
