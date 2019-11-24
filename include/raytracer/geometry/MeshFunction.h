#ifndef RAYTRACER_MESH_FUNCTION_H
#define RAYTRACER_MESH_FUNCTION_H

#include <map>

#include "raytracer/utility/JsonFormatter.h"
#include "Mesh.h"
#include "Quadrilateral.h"

namespace raytracer {
    namespace impl {
        class MeshFunctionGe;
    }

    namespace geometry {
        class MeshFunction {
        public:
            explicit MeshFunction(const Mesh& mesh): mesh(mesh) {
                const auto& quads = mesh.getQuads();
                this->values.resize(quads.size());
            }

            impl::MeshFunctionGe greaterOrEqual(
                    double threshold,
                    const std::function<void(geometry::Quadrilateral)>& callback);

            template <typename Function>
            void setAll(const std::vector<Quadrilateral>& quadrilaterals, Function function){
                for (const auto& quad : quadrilaterals){
                    this->values[quad.id] = function(quad);
                }
            }

            const std::vector<double>& getValues() const {
                return this->values;
            }

            double& operator[] (const Quadrilateral& quad){
                return this->values[quad.id];
            }

            const double& operator[] (const Quadrilateral& quad) const {
                return this->values.at(quad.id);
            }

            utility::json::Value getJsonValue() const {
                using JsonValue = utility::json::Value;

                JsonValue points;

                for (const auto& quad : this->mesh.getQuads()) {
                    auto center = quad.getAveragePoint();
                    JsonValue point;
                    point.append(JsonValue(center.x));
                    point.append(JsonValue(center.y));
                    point.append(this->values[quad.id]);

                    points.append(point);
                }
                return points;
            }

        private:
            std::vector<double> values;
            const Mesh& mesh;
        };
    }

    namespace impl {
        class MeshFunctionGe {
        public:
            MeshFunctionGe(
                    const geometry::MeshFunction &meshFunction,
                    double threshold,
                    std::function<void(geometry::Quadrilateral)> callback):
                    meshFunction(meshFunction), threshold(threshold), callback(std::move(callback)) {}

            bool operator()(const geometry::Quadrilateral &quad) {
                auto value = meshFunction[quad];
                if (value >= threshold) {
                    callback(quad);
                    return true;
                } else {
                    return false;
                }
            }

        private:
            const geometry::MeshFunction &meshFunction;
            double threshold;
            std::function<void(geometry::Quadrilateral)> callback;
        };
    }
}



#endif //RAYTRACER_MESH_FUNCTION_H
