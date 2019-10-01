import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import pandas as pd

import sys



# File RES_FILE_CORE_VS_FREQ"
X, Y, Z = np.loadtxt(fname = sys.argv[1], unpack=True)
df = pd.DataFrame({'x': X, 'y': Y, 'z': Z})


fig = plt.figure()
ax = Axes3D(fig)



# exit (1)

# Annotate the maximum value with a red line and its value
# Can have multiple maximum
max = df[df['z']==df['z'].max()]
for index, row in max.iterrows():
    print ("Annotate maximum bandwidth at (" + str(row['x']) + (",") + str(row['y']) + (",") + str(row['z']) + ")")
    VecStart_x = [row['x']]
    VecStart_y = [row['y']]
    VecStart_z = [0]
    VecEnd_x = [row['x']]
    VecEnd_y = [row['y']]
    VecEnd_z  =[row['z']]
    ax.plot([VecStart_x[0], VecEnd_x[0]], [VecStart_y[0],VecEnd_y[0]],zs=[VecStart_z[0],VecEnd_z[0]], c='red')
    ax.text(row['x'],row['y'],0, "(" + str(row['x']) + (",") + str(row['y']) + (",") + str(row['z']) + ")" )



surf = ax.plot_trisurf(df.x, df.y, df.z, cmap=cm.jet, linewidth=0.01, vmin=df.z.quantile(0.7))
fig.colorbar(surf, shrink=0.5, aspect=5, label="Memory bandwidth (GB/s)")
ax.set_xlabel('Number of Cores')
ax.set_ylabel('Frequencies')
ax.set_zlabel('Memory bandwidth (GB/s)')
ax.set_xlim([0,20])
ax.set_ylim([0,3.7])

plt.show()




