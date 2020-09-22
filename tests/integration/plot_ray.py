#!/usr/bin/env python3

import matplotlib.pyplot as plt
import rayvis
import matplotlib.ticker as ticker

if __name__ == "__main__":
    fig, axis = plt.subplots()
    rays = rayvis.read_rays("data/ray1.json")
    rayvis.plot_rays(axis, rays, color="orange", linestyle="solid", linewidth=1, label="$\\theta_0 = 0.1$ rad")

    x, y = rayvis.analytic_solution(rays[0][0][0], rays[0][0][1],
                                    rays[0][1][0] - rays[0][0][0],
                                    rays[0][1][1] - rays[0][0][1])
    plt.plot(x, y, linestyle="dashed", color="skyblue", label="$\\theta_0 = 0.1$ rad analytic")

    rays = rayvis.read_rays("data/ray2.json")
    rayvis.plot_rays(axis, rays, color="springgreen", linewidth=1, linestyle="dashdot", label="$\\theta_0 = 0.2$ rad")

    x, y = rayvis.analytic_solution(rays[0][0][0], rays[0][0][1],
                                    rays[0][1][0] - rays[0][0][0],
                                    rays[0][1][1] - rays[0][0][1])
    plt.plot(x, y, linestyle="dotted", color="violet", label="$\\theta_0 = 0.2$ rad analytic")

    nodes, elements = rayvis.read_vtk("data/mesh.vtk")
    rayvis.plot_vtk_mesh(axis, nodes, elements)

    values = rayvis.read_grid_function("data/density.txt")
    rayvis.plot_grid_function(fig, axis, nodes, elements, values, "$n_e$ [cm$^{-3}$]")


    formatter = ticker.ScalarFormatter(useMathText=True)
    formatter.set_scientific(True)
    formatter.set_powerlimits((-2, 2))
    axis.yaxis.set_major_formatter(formatter)
    axis.xaxis.set_major_formatter(formatter)
    axis.set_xlabel("$z$ [-]")
    axis.set_ylabel("$y$ [-]")

    plt.legend(fancybox=True, framealpha=1)
    plt.axis('equal')
    plt.savefig("images/ray1.eps", format='eps', dpi=1200)
    plt.clf()

    fig, axis = plt.subplots()
    rays = rayvis.read_rays("data/ray3.json")
    rayvis.plot_rays(axis, rays, color="orange", linestyle="solid", linewidth=1, label="$\\theta_0 = 0.25$ rad")

    x, y = rayvis.analytic_solution(rays[0][0][0], rays[0][0][1],
                                    rays[0][1][0] - rays[0][0][0],
                                    rays[0][1][1] - rays[0][0][1])
    plt.plot(x, y,  linestyle="dashed", color="skyblue", label="$\\theta_0 = 0.25$ rad analytic")

    rays = rayvis.read_rays("data/ray4.json")
    rayvis.plot_rays(axis, rays, color="springgreen", linewidth=1, linestyle="dashdot", label="$\\theta_0 = 0.5$ rad")

    x, y = rayvis.analytic_solution(rays[0][0][0], rays[0][0][1],
                                    rays[0][1][0] - rays[0][0][0],
                                    rays[0][1][1] - rays[0][0][1])
    plt.plot(x, y, linestyle="dotted", color="violet", label="$\\theta_0 = 0.5$ rad analytic")

    nodes, elements = rayvis.read_vtk("data/mesh.vtk")
    rayvis.plot_vtk_mesh(axis, nodes, elements)

    values = rayvis.read_grid_function("data/density.txt")
    rayvis.plot_grid_function(fig, axis, nodes, elements, values, "$n_e$ [cm$^{-3}$]")

    axis.yaxis.set_major_formatter(formatter)
    axis.xaxis.set_major_formatter(formatter)
    axis.set_xlabel("$z$ [-]")
    axis.set_ylabel("$y$ [-]")

    plt.legend(fancybox=True, framealpha=1)

    plt.axis('equal')
    plt.savefig("images/ray2.eps", format='eps', dpi=1200)
    plt.clf()
