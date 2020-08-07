import rayvis
import sys
from matplotlib import pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm


def analytic_gradient(x0, y0):
    return 2 * np.pi * np.cos(2 * np.pi * x0), 2 * np.pi * np.cos(2 * np.pi * y0)
    # return np.divide(1, (np.power(x0, 2) - x0 + 1.25)), y0*0


def save_compare_gradients(x, y, discrete_grad, analytic_grad, image_filename, title):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_wireframe(x, y, analytic_grad, alpha=0.5)
    surf = ax.plot_surface(x, y, discrete_grad, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False, alpha=0.6)
    fig.colorbar(surf, shrink=0.5, aspect=5)
    ax.set_xlabel('x [cm]')
    ax.set_ylabel('y [cm]')
    ax.set_zlabel('grad($\\rho$ cm$^{-4}$)')
    ax.set_title(title)
    plt.savefig(image_filename)
    plt.clf()


def save_error(x, y, discrete_grad, analytic_grad, image_filename, title):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    surf = ax.plot_surface(x, y, analytic_grad - discrete_grad, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False)
    fig.colorbar(surf, shrink=0.5, aspect=5)
    ax.set_xlabel('x [cm]')
    ax.set_ylabel('y [cm]')
    ax.set_zlabel('$\\Delta$ grad($\\rho$ cm$^{-4}$)')
    ax.set_title(title)
    plt.savefig(image_filename)
    plt.clf()


def main():
    sizeData =  np.genfromtxt("data/gridSize.csv", delimiter=",")
    startX, stopX, countX = sizeData[0]
    startY, stopY, countY = sizeData[1]
    x = np.linspace(startX, stopX, int(countX))
    y = np.linspace(startY, stopY, int(countY))
    x, y = np.meshgrid(x, y)

    analytic_grad_x, analytic_grad_y = analytic_gradient(x, y)

    _, axis = plt.subplots()
    rayvis.plot_vtk_mesh(axis, *rayvis.read_vtk("data/mesh.vtk"))
    axis.plot(x, y, marker="+", linestyle="", color="r")

    axis.axis('equal')
    plt.savefig("images/mesh.png")
    plt.clf()

    grad_x = np.genfromtxt("data/least_square_x.csv", delimiter=",")
    save_compare_gradients(x, y, grad_x, analytic_grad_x, "images/ls_grad_x_compare.png",
                           "Least squares (grad($\\rho$))$_x$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$")
    save_error(x, y, grad_x, analytic_grad_x, "images/ls_error_x.png",
               "Least squares (grad($\\rho$))$_x$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$ error")

    grad_y = np.genfromtxt("data/least_square_y.csv", delimiter=",")
    save_compare_gradients(x, y, grad_y, analytic_grad_y, "images/ls_grad_y_compare.png",
                           "Least squares (grad($\\rho$))$_y$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$")
    save_error(x, y, grad_y, analytic_grad_y, "images/ls_error_y.png",
               "Least squares (grad($\\rho$))$_y$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$ error")

    grad_x = np.genfromtxt("data/h1_gradient_x.csv", delimiter=",")
    save_compare_gradients(x, y, grad_x, analytic_grad_x, "images/h1_grad_x_compare.png",
                           "$H1$ (grad($\\rho$))$_x$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$")
    save_error(x, y, grad_x, analytic_grad_x, "images/h1_error_x.png",
               "$H1$ (grad($\\rho$))$_x$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$ error")

    grad_y = np.genfromtxt("data/h1_gradient_y.csv", delimiter=",")
    save_compare_gradients(x, y, grad_y, analytic_grad_y, "images/h1_grad_y_compare.png",
                           "$H1$ (grad($\\rho$))$_y$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$")
    save_error(x, y, grad_y, analytic_grad_y, "images/h1_error_y.png",
               "$H1$ (grad($\\rho$))$_y$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$ error")

    grad_x = np.genfromtxt("data/householder_x.csv", delimiter=",")
    save_compare_gradients(x, y, grad_x, analytic_grad_x, "images/householder_grad_x_compare.png",
                           "$HH$ (grad($\\rho$))$_x$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$")
    save_error(x, y, grad_x, analytic_grad_x, "images/householder_error_x.png",
               "$HH$ (grad($\\rho$))$_x$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$ error")

    grad_y = np.genfromtxt("data/householder_y.csv", delimiter=",")
    save_compare_gradients(x, y, grad_y, analytic_grad_y, "images/householder_y_compare.png",
                           "$HH$ (grad($\\rho$))$_y$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$")
    save_error(x, y, grad_y, analytic_grad_y, "images/householder_error_y.png",
               "$HH$ (grad($\\rho$))$_y$ of $\\rho(x, y) = sin(2 \\pi x) + sin(2 \\pi y)$ error")


if __name__ == "__main__":
    main()
