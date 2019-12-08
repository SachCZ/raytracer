#ifndef RAYTRACER_LASER_RAY_H
#define RAYTRACER_LASER_RAY_H

#include <cmath>
#include <vector>

#include "Ray.h"
#include "Magnitudes.h"
#include "Point.h"
#include "Vector.h"
#include "Constants.h"
#include "GeometryFunctions.h"

namespace raytracer {
    namespace physics {

        /**
         * Structure representing a single LaserRay. It has associated energy but no width for now.
         */
        struct LaserRay {
            geometry::Point startPoint{};
            geometry::Vector direction{};
            Energy energy{};
            Length wavelength{};

            Density getCriticalDensity() const;

            template<typename IntersFunc, typename StopCondition>
            void generateIntersections(
                    const geometry::Mesh &mesh,
                    IntersFunc findInters,
                    StopCondition stopCondition) {
                geometry::Ray ray(geometry::HalfLine{this->startPoint, this->direction});

                this->intersections = ray.findIntersections(mesh, findInters, stopCondition);
            }

            std::vector<geometry::Intersection> intersections;
        };
    }
}

#endif //RAYTRACER_LASER_RAY_H
