

from matplotlib import pyplot as plt

import numpy as np

interface_data = np.loadtxt("i_data.txt", delimiter=" ")

direct_access_data = np.loadtxt("da_data.txt", delimiter=" ")

plt.plot(*np.transpose(direct_access_data), label="Direct Access")
plt.plot(*np.transpose(interface_data), label="Interface")

plt.style.use('fivethirtyeight')
plt.yscale('log')
plt.title("MOAB connectivity/coordinate query scaling")
plt.ylabel("Queries per second")
plt.xlabel("# threads")
plt.ylim((1e6,1e9))
plt.grid()
plt.legend()

plt.savefig("scaling_comparison.png")
plt.show()
