import matplotlib.pyplot as plt


# Données
data = """
Execution time for 1 core(s): 4.946830202 seconds
Execution time for 2 core(s): 4.958343077 seconds
Execution time for 3 core(s): 4.968569576 seconds
Execution time for 4 core(s): 4.976589316 seconds
Execution time for 5 core(s): 4.990270100 seconds
Execution time for 6 core(s): 4.994109311 seconds
Execution time for 7 core(s): 5.003614916 seconds
Execution time for 8 core(s): 5.012554922 seconds
Execution time for 9 core(s): 5.021271491 seconds
Execution time for 10 core(s): 5.029197036 seconds
Execution time for 11 core(s): 5.044757322 seconds
Execution time for 12 core(s): 5.045205646 seconds
Execution time for 13 core(s): 5.055423850 seconds
Execution time for 14 core(s): 5.066496020 seconds
Execution time for 15 core(s): 5.067267175 seconds
Execution time for 16 core(s): 5.083027455 seconds
Execution time for 17 core(s): 5.081049610 seconds
Execution time for 18 core(s): 5.106353633 seconds
Execution time for 19 core(s): 5.113940972 seconds
Execution time for 20 core(s): 5.114743356 seconds
Execution time for 21 core(s): 5.132107157 seconds
Execution time for 22 core(s): 5.139169184 seconds
Execution time for 23 core(s): 5.154109216 seconds
Execution time for 24 core(s): 5.152339356 seconds
Execution time for 25 core(s): 5.166040811 seconds
Execution time for 26 core(s): 5.157633307 seconds
Execution time for 27 core(s): 5.194230584 seconds
Execution time for 28 core(s): 5.404238286 seconds
Execution time for 29 core(s): 5.679098074 seconds
Execution time for 30 core(s): 5.686681179 seconds
Execution time for 31 core(s): 5.697465824 seconds
Execution time for 32 core(s): 5.707440954 seconds
Execution time for 33 core(s): 5.721380703 seconds
Execution time for 34 core(s): 5.918210335 seconds
Execution time for 35 core(s): 6.111298846 seconds
Execution time for 36 core(s): 6.116508627 seconds
Execution time for 37 core(s): 6.127975434 seconds
Execution time for 38 core(s): 6.134490957 seconds
Execution time for 39 core(s): 6.144906177 seconds
Execution time for 40 core(s): 6.564354885 seconds
Execution time for 41 core(s): 7.089821591 seconds
Execution time for 42 core(s): 7.100434243 seconds
Execution time for 43 core(s): 7.108412614 seconds
Execution time for 44 core(s): 7.267469115 seconds
Execution time for 45 core(s): 7.395983490 seconds
Execution time for 46 core(s): 7.404045195 seconds
Execution time for 47 core(s): 7.414355405 seconds
Execution time for 48 core(s): 7.559841063 seconds
Execution time for 49 core(s): 7.708289310 seconds
Execution time for 50 core(s): 7.720140287 seconds
Execution time for 51 core(s): 7.725656626 seconds
Execution time for 52 core(s): 7.735169212 seconds
Execution time for 53 core(s): 7.737585906 seconds
Execution time for 54 core(s): 7.742324038 seconds
Execution time for 55 core(s): 7.745433037 seconds
Execution time for 56 core(s): 7.758442476 seconds
"""


cores = []
times = []
for line in data.strip().split("\n"):
    core, time = line.split(":")
    cores.append(int(core.split()[3]))
    times.append(float(time.split()[0]))


fig, ax = plt.subplots(figsize=(10, 6))
ax.plot(cores, times, '-o', label="Execution time (s)", color='blue', markersize=8, markeredgecolor='black')
ax.set_title('Execution time for 1 to 56 cores on the same socket', fontdict={'size': 16})
ax.set_xlabel('Number of core (same socket)', fontsize=14)
ax.set_ylabel('Time (second)', fontsize=14)
ax.tick_params(labelsize=12)
ax.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
ax.legend(fontsize=12)
plt.tight_layout()
plt.show()
