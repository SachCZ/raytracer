#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis

if __name__ == "__main__":
    _, axis = plt.subplots()
    rayvis.plot_rays(axis, rayvis.read_rays("data/rays.json"))

    nodes, elements = rayvis.read_vtk("data/mesh.vtk")
    rayvis.plot_vtk_mesh(axis, nodes, elements)

    plt.axis('equal')
    plt.savefig("images/rays.png")
    plt.clf()

    fig, axis = plt.subplots()
    values = rayvis.read_grid_function("data/absorbed_energy.txt")
    rayvis.plot_grid_function(fig, axis, nodes, elements, values)
    plt.axis('equal')
    plt.savefig("images/absorbed_energy.png")
