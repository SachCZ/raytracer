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

raytracer::MeshFunc::Ptr MeshFunctionMock::calcTransformed(const raytracer::MeshFunc::Transform & func) const {
    auto result = std::make_unique<MeshFunctionMock>(func(raytracer::Element{0, {}}));
    for (const auto& pair : this->values){
        auto element = raytracer::Element{pair.first, {}};
        result->setValue(element, func(element));
    }
    return result;
}
