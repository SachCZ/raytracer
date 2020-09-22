#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis
import matplotlib.ticker as ticker
import numpy as np

if __name__ == "__main__":
    formatter = ticker.ScalarFormatter(useMathText=True)
    formatter.set_scientific(True)
    formatter.set_powerlimits((-3,3))
    nodes, elements = rayvis.read_mesh("data/mesh_long.mesh")
    nodes = nodes * 1e4

    fig, axis = plt.subplots()
    values = rayvis.read_grid_function("data/gain.txt")

    rayvis.plot_grid_function(fig, axis, nodes, elements, values, cmap='plasma', label="$g$ [cm$^{-1}$]")
    rayvis.plot_vtk_mesh(axis, nodes, elements, edgecolor="blue", facecolors="none")


    axis.yaxis.set_major_formatter(formatter)
    axis.xaxis.set_major_formatter(formatter)
    axis.set_xlabel("$x$ [nm]")
    axis.set_ylabel("$y$ [nm]")

    axis.set_ylim([0, 50])

    plt.savefig("images/gain_mesh.png")


