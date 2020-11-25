import rayvis
import numpy as np
from matplotlib import pyplot as plt


def analytic_fun(x, y):
    return np.divide(0.1, x ** 2 - x + 0.26), np.divide(0.1, y ** 2 - y + 0.26)


def main():
    with open("householder.msgpack", "rb") as f:
        vector_field = rayvis.read_vector_field(f)

    error = vector_field - analytic_fun(vector_field.coord_x, vector_field.coord_y)
    grad_norm = np.asarray(2*[vector_field.norm()])

    with open("dualMesh.mfem") as f:
        dual_mesh = rayvis.read_mfem_mesh(f)

    fig, axes = plt.subplots()
    contour = rayvis.plot_vector_field(axes, error / grad_norm * np.asarray([100, 100]), dual_mesh)
    color_bar = fig.colorbar(contour)
    color_bar.ax.set_ylabel("$|\\Delta \\vec{G}|$ [%]")
    plt.show()


if __name__ == '__main__':
    main()
