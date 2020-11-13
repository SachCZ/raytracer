import msgpack as msgpack
import numpy as np
from matplotlib import pyplot as plt

if __name__ == '__main__':
    with open("householder.msgpack", "rb") as data_file:
        byte_data = data_file.read()

    data_loaded = np.asarray(msgpack.unpackb(byte_data))
    x, y, grad_x, grad_y = np.rollaxis(data_loaded, 1)
    plt.quiver(x, y, grad_x, grad_y)
    plt.show()
