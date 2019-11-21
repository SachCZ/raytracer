#ifndef RAYTRACER_MESH_FUNCTION_H
#define RAYTRACER_MESH_FUNCTION_H

#include <map>

#include "Mesh.h"
#include "Quadrilateral.h"

namespace raytracer {
    namespace geometry {
        class MeshFunction {
        public:
            explicit MeshFunction(const Mesh& mesh) {
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

        private:
            std::vector<double> values;
        };
    }
}



#endif //RAYTRACER_MESH_FUNCTION_H
