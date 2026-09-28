#include "shared_types.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

int main()
{
    // Один спільний набір вхідних даних для обох алгоритмів.
    // Табличні значення взяті для f(x)=sin(x).
    const double x0 = 1.5;

    auto data = std::make_shared<const InputData>(
        InputData{
            std::vector<Point>{
                {0.0, std::sin(0.0)},
                {0.5, std::sin(0.5)},
                {1.0, std::sin(1.0)},
                {2.0, std::sin(2.0)},
                {2.5, std::sin(2.5)},
                {3.0, std::sin(3.0)}
            },
            x0,
            std::sin(x0)
        });

    auto resultA = calculateA(data);
    auto resultB = calculateB(data);

    // Обов'язкові structured bindings.
    auto [valueA, errorA, operationsA, timeA] = *resultA;
    auto [valueB, errorB, operationsB, timeB] = *resultB;

    std::cout << std::fixed << std::setprecision(10);
    std::cout << "Variant 3: interpolation of a tabulated function\n";
    std::cout << "x0 = " << data->x0 << "\n";
    std::cout << "Reference sin(x0) = " << data->referenceValue << "\n\n";

    std::cout << "Student A - Lagrange interpolation\n";
    std::cout << "Value:      " << valueA << "\n";
    std::cout << "Abs. error: " << errorA << "\n";
    std::cout << "Operations: " << operationsA << "\n";
    std::cout << "Time, us:   " << timeA << "\n\n";

    std::cout << "Student B - Natural cubic spline interpolation\n";
    std::cout << "Value:      " << valueB << "\n";
    std::cout << "Abs. error: " << errorB << "\n";
    std::cout << "Operations: " << operationsB << "\n";
    std::cout << "Time, us:   " << timeB << "\n\n";

    std::cout << "Difference between methods: "
              << std::abs(valueA - valueB) << "\n";

    return 0;
}
