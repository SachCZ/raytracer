#ifndef RAYTRACER_MOCKS_H
#define RAYTRACER_MOCKS_H

#include <geometry.h>

class MeshFunctionMock : public raytracer::MeshFunction {
public:
    double getValue(const raytracer::Element &element) const override {
        return this->values.at(element.getId());
    }

    void setValue(const raytracer::Element &element, double value) override {
        values[element.getId()] = value;
    }

    void addValue(const raytracer::Element &element, double value) override {
        auto it = values.find(element.getId());
        if (it == values.end()) throw std::runtime_error("Element does not exist");
        it->second += value;
    }

private:
    std::map<int, double> values;
};


#endif //RAYTRACER_MOCKS_H
