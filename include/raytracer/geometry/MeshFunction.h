#ifndef RAYTRACER_MESH_FUNCTION_H
#define RAYTRACER_MESH_FUNCTION_H

#include <map>

#include "raytracer/utility/JsonFormatter.h"
#include "Mesh.h"
#include "Quadrilateral.h"

namespace raytracer {
    namespace geometry {
        class MeshFunction {
        public:
            explicit MeshFunction(const Mesh& mesh): mesh(mesh) {
                const auto& quads = mesh.getQuads();
                this->values.resize(quads.size());
            }

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
}



#endif //RAYTRACER_MESH_FUNCTION_H
