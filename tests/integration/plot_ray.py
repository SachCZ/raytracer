#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis
import matplotlib.ticker as ticker

if __name__ == "__main__":
    fig, axis = plt.subplots()
    rays = rayvis.read_rays("data/ray.json")
    rayvis.plot_rays(axis, rays, color="red", linewidth=1)

    nodes, elements = rayvis.read_vtk("data/mesh.vtk")
    rayvis.plot_vtk_mesh(axis, nodes, elements)

    values = rayvis.read_grid_function("data/density.txt")
    rayvis.plot_grid_function(fig, axis, nodes, elements, values)

    x, y = rayvis.analytic_solution(rays[0][0][0], rays[0][0][1],
                                    rays[0][1][0] - rays[0][0][0],
                                    rays[0][1][1] - rays[0][0][1])
    plt.plot(x, y, linestyle="--")

    formatter = ticker.ScalarFormatter(useMathText=True)
    formatter.set_scientific(True)
    formatter.set_powerlimits((-1, 1))
    axis.yaxis.set_major_formatter(formatter)
    axis.xaxis.set_major_formatter(formatter)
    axis.set_xlabel("$x$ [cm]")
    axis.set_ylabel("$y$ [cm]")

    plt.axis('equal')
    plt.savefig("images/ray.svg", format='svg', dpi=1200)
    plt.clf()
