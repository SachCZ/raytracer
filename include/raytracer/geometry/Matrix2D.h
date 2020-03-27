#ifndef RAYTRACER_MATRIX2D_H
#define RAYTRACER_MATRIX2D_H

namespace raytracer {
    /**
     * Simple 2D matrix representation
     */
    struct Matrix2D {
        /** Element at 1,1 */
        double xx;
        /** Element at 1,2 */
        double xy;
        /** Element at 2,1 */
        double yx;
        /** Element at 2,2 */
        double yy;
    };
}

#endif //RAYTRACER_MATRIX2D_H
