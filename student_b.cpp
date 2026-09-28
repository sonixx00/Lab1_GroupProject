#include "shared_types.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <vector>

std::unique_ptr<Result> calculateB(std::shared_ptr<const InputData> data)
{
    if (!data || data->points.size() < 2)
        throw std::invalid_argument("At least two points are required.");

    const auto start = std::chrono::high_resolution_clock::now();

    // Працюємо з копією точок, щоб не змінювати shared_ptr<const InputData>.
    std::vector<Point> p = data->points;
    std::sort(p.begin(), p.end(),
              [](const Point& a, const Point& b) { return a.x < b.x; });

    const std::size_t n = p.size();
    std::size_t operations = 0;

    for (std::size_t i = 1; i < n; ++i)
    {
        if (std::abs(p[i].x - p[i - 1].x) < 1e-12)
            throw std::invalid_argument("Duplicate x values are not allowed.");
    }

    if (data->x0 < p.front().x || data->x0 > p.back().x)
        throw std::out_of_range("x0 must lie inside the interpolation interval.");

    // Натуральний кубічний сплайн.
    // a[i] = y[i], b[i], c[i], d[i] - коефіцієнти на [x_i, x_{i+1}].
    std::vector<double> a(n), b(n - 1), c(n), d(n - 1), h(n - 1);
    std::vector<double> alpha(n, 0.0), l(n), mu(n, 0.0), z(n, 0.0);

    for (std::size_t i = 0; i < n; ++i)
        a[i] = p[i].y;

    for (std::size_t i = 0; i + 1 < n; ++i)
    {
        h[i] = p[i + 1].x - p[i].x;
        ++operations;
    }

    for (std::size_t i = 1; i + 1 < n; ++i)
    {
        alpha[i] =
            (3.0 / h[i]) * (a[i + 1] - a[i]) -
            (3.0 / h[i - 1]) * (a[i] - a[i - 1]);
        operations += 10;
    }

    // Натуральні граничні умови: S''(x_0)=S''(x_n)=0.
    l[0] = 1.0;
    c[0] = 0.0;
    z[0] = 0.0;

    for (std::size_t i = 1; i + 1 < n; ++i)
    {
        l[i] = 2.0 * (p[i + 1].x - p[i - 1].x) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
        operations += 9;
    }

    l[n - 1] = 1.0;
    z[n - 1] = 0.0;
    c[n - 1] = 0.0;

    for (std::size_t j = n - 1; j-- > 0;)
    {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] =
            (a[j + 1] - a[j]) / h[j] -
            h[j] * (c[j + 1] + 2.0 * c[j]) / 3.0;
        d[j] = (c[j + 1] - c[j]) / (3.0 * h[j]);
        operations += 12;
    }

    std::size_t interval = n - 2;
    for (std::size_t i = 0; i + 1 < n; ++i)
    {
        if (data->x0 >= p[i].x && data->x0 <= p[i + 1].x)
        {
            interval = i;
            break;
        }
    }

    const double dx = data->x0 - p[interval].x;
    const double value =
        a[interval] +
        b[interval] * dx +
        c[interval] * dx * dx +
        d[interval] * dx * dx * dx;
    operations += 10;

    const auto finish = std::chrono::high_resolution_clock::now();
    const double timeUs =
        std::chrono::duration<double, std::micro>(finish - start).count();

    const double error = std::abs(value - data->referenceValue);

    return std::make_unique<Result>(
        Result{value, error, operations, timeUs});
}
