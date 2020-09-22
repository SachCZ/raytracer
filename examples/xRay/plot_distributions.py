import numpy as np
from matplotlib import pyplot as plt


def get_intensity(x, energy, N):
    N = 100
    start = min(x) - 0.0005
    stop = max(x) + 0.0005
    ds = (stop - start) / (N - 1)
    intensity = []
    coordiantes = np.linspace(start, stop, N)
    for x0 in coordiantes:
        mask = (x > (x0 - ds / 2))*(x <= (x0 + ds / 2))
        intensity.append(sum(energy[mask])/ds)
    print(sum(np.asarray(intensity) * ds))
    return coordiantes, np.asarray(intensity)


def main():
    initial_x, initial_y, initial_energy = np.genfromtxt("data/initial_distribution.csv", unpack=True, delimiter=",")
    final_x, final_y, final_energy = np.genfromtxt("data/final_distribution.csv", unpack=True, delimiter=",")

    x0, initial_intensity = get_intensity(initial_x, initial_energy, 100)
    x1, final_intensity = get_intensity(final_x, final_energy, 100)

    plt.plot(x0, initial_intensity)# / max(initial_intensity))
    plt.plot(x1, final_intensity)# / max(final_intensity))
    plt.show()


if __name__ == '__main__':
    main()
