#include "mocks.h"

MeshFunctionMock::MeshFunctionMock(double defaultValue) : defaultValue(defaultValue) {}

double MeshFunctionMock::getValue(const raytracer::Element &element) const {
    auto it = values.find(element.getId());
    if (it == values.end()) return defaultValue;
    return it->second;
}

void MeshFunctionMock::setValue(const raytracer::Element &element, double value) {
    values[element.getId()] = value;
}

void MeshFunctionMock::addValue(const raytracer::Element &element, double value) {
    auto it = values.find(element.getId());
    if (it == values.end()) throw std::runtime_error("Element does not exist");
    it->second += value;
}
