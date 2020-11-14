import msgpack as msgpack
import numpy as np
from matplotlib import pyplot as plt


def analytic(x, y):
    return np.divide(0.1, x ** 2 - x + 0.26), np.divide(0.1, y ** 2 - y + 0.26)


def plot_vector_field(axes, x, y, field_x, field_y):
    norm = np.sqrt(field_x ** 2 + field_y ** 2)
    contour = axes.tricontourf(x, y, norm, levels=1000, cmap="jet")
    axes.quiver(x, y, field_x, field_y, scale=12 * max(norm), color="white")
    return contour


def main():
    with open("householder.msgpack", "rb") as data_file:
        byte_data = data_file.read()

    data_loaded = np.asarray(msgpack.unpackb(byte_data))
    x, y, grad_x, grad_y = np.rollaxis(data_loaded, 1)
    analytic_grad = analytic(x, y)
    grad_x_error = grad_x - analytic_grad[0]
    grad_y_error = grad_y - analytic_grad[1]
    grad_norm = np.sqrt(grad_x ** 2 + grad_y ** 2)

    fig, axes = plt.subplots()
    contour = plot_vector_field(axes, x, y, grad_x_error / grad_norm * 100, grad_y_error / grad_norm * 100)
    colorbar = fig.colorbar(contour)
    colorbar.ax.set_ylabel("$|\\Delta \\vec{G}|$ [%]")
    plt.show()


if __name__ == '__main__':
    main()
