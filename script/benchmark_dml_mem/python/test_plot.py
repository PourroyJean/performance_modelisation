import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import pandas as pd



X, Y, Z = np.loadtxt(fname = "/Users/jean/Desktop/codesshfs2/script/python/RES_FILE_CORE_VS_FREQ", unpack=True)

df = pd.DataFrame({'x': X, 'y': Y, 'z': Z})
Z = df.pivot(index='x', columns='y', values='z')
X, Y = np.meshgrid(X, Y)

fig = plt.figure()
ax = Axes3D(fig)
surf = ax.plot_surface(df.x, df.y, df.z, cmap=cm.jet, linewidth=0.01, vmin=100)
fig.colorbar(surf, shrink=0.5, aspect=5)


plt.show()
