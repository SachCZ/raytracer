#ifndef RAYTRACER_MOCKS_H
#define RAYTRACER_MOCKS_H

#include <geometry.h>

class MeshFunctionMock : public raytracer::MeshFunc {
public:
    MeshFunctionMock(double defaultValue = 0);

    double getValue(const raytracer::Element &element) const override;

    void setValue(const raytracer::Element &element, double value) override;

    void addValue(const raytracer::Element &element, double value) override;

    Ptr calcTransformed(const Transform & func) const override;

private:
    double defaultValue;
    std::map<int, double> values;
};


#endif //RAYTRACER_MOCKS_H
