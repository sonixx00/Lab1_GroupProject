#pragma once

#include <vector>
#include <memory>
#include <cstddef>

struct Point
{
    double x;
    double y;
};

struct InputData
{
    std::vector<Point> points;
    double x0;
    double referenceValue; // точне/еталонне значення лише для тестової оцінки похибки
};

struct Result
{
    double value;
    double error;
    std::size_t operations;
    double timeMicroseconds;
};

std::unique_ptr<Result> calculateA(std::shared_ptr<const InputData> data);
std::unique_ptr<Result> calculateB(std::shared_ptr<const InputData> data);
