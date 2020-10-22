import rayvis
from matplotlib import pyplot as plt


if __name__ == '__main__':
    with open("mesh.mfem") as f:
        mesh = rayvis.read_mfem_mesh(f)
    with open("trajectory.msgpack", "rb") as f:
        rays = rayvis.read_msgpack_rays(f)
    fig, axes = plt.subplots()
    rayvis.plot_mesh(axes, mesh, linewidth=0.5)
    rayvis.plot_rays(axes, rays, linewidth=0.5, color="red")
    plt.show()
