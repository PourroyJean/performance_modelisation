import matplotlib.pyplot as plt


# Données
data = """
Execution time for 2 core(s) [1 from each socket]: 4.993321803 seconds
Execution time for 4 core(s) [2 from each socket]: 5.011354868 seconds
Execution time for 6 core(s) [3 from each socket]: 5.018517870 seconds
Execution time for 8 core(s) [4 from each socket]: 5.039287356 seconds
Execution time for 10 core(s) [5 from each socket]: 5.051478906 seconds
Execution time for 12 core(s) [6 from each socket]: 5.063865721 seconds
Execution time for 14 core(s) [7 from each socket]: 5.081821746 seconds
Execution time for 16 core(s) [8 from each socket]: 5.095461196 seconds
Execution time for 18 core(s) [9 from each socket]: 5.112967852 seconds
Execution time for 20 core(s) [10 from each socket]: 5.131092615 seconds
Execution time for 22 core(s) [11 from each socket]: 5.185493649 seconds
Execution time for 24 core(s) [12 from each socket]: 5.171073862 seconds
Execution time for 26 core(s) [13 from each socket]: 5.191509391 seconds
Execution time for 28 core(s) [14 from each socket]: 5.197612572 seconds
Execution time for 30 core(s) [15 from each socket]: 5.217382604 seconds
Execution time for 32 core(s) [16 from each socket]: 5.245168887 seconds
Execution time for 34 core(s) [17 from each socket]: 5.255028058 seconds
Execution time for 36 core(s) [18 from each socket]: 5.281487144 seconds
Execution time for 38 core(s) [19 from each socket]: 5.310311524 seconds
Execution time for 40 core(s) [20 from each socket]: 5.326569733 seconds
Execution time for 42 core(s) [21 from each socket]: 5.346056135 seconds
Execution time for 44 core(s) [22 from each socket]: 5.377028534 seconds
Execution time for 46 core(s) [23 from each socket]: 5.388823628 seconds
Execution time for 48 core(s) [24 from each socket]: 5.401037107 seconds
Execution time for 50 core(s) [25 from each socket]: 5.421567269 seconds
Execution time for 52 core(s) [26 from each socket]: 5.442994639 seconds
Execution time for 54 core(s) [27 from each socket]: 5.477415395 seconds
Execution time for 56 core(s) [28 from each socket]: 5.703866244 seconds
Execution time for 58 core(s) [29 from each socket]: 5.996169311 seconds
Execution time for 60 core(s) [30 from each socket]: 6.006538129 seconds
Execution time for 62 core(s) [31 from each socket]: 6.022999940 seconds
Execution time for 64 core(s) [32 from each socket]: 6.045178112 seconds
Execution time for 66 core(s) [33 from each socket]: 6.060168108 seconds
Execution time for 68 core(s) [34 from each socket]: 6.276444985 seconds
Execution time for 70 core(s) [35 from each socket]: 6.482776121 seconds
Execution time for 72 core(s) [36 from each socket]: 6.497327588 seconds
Execution time for 74 core(s) [37 from each socket]: 6.536924348 seconds
Execution time for 76 core(s) [38 from each socket]: 6.543342176 seconds
Execution time for 78 core(s) [39 from each socket]: 6.576751028 seconds
Execution time for 80 core(s) [40 from each socket]: 7.000506446 seconds
Execution time for 82 core(s) [41 from each socket]: 7.547284814 seconds
Execution time for 84 core(s) [42 from each socket]: 7.553418395 seconds
Execution time for 86 core(s) [43 from each socket]: 7.599593032 seconds
Execution time for 88 core(s) [44 from each socket]: 7.737069784 seconds
Execution time for 90 core(s) [45 from each socket]: 7.906064873 seconds
Execution time for 92 core(s) [46 from each socket]: 7.950340764 seconds
Execution time for 94 core(s) [47 from each socket]: 7.956628009 seconds
Execution time for 96 core(s) [48 from each socket]: 8.129205157 seconds
Execution time for 98 core(s) [49 from each socket]: 8.509895560 seconds
Execution time for 100 core(s) [50 from each socket]: 8.343178223 seconds
Execution time for 102 core(s) [51 from each socket]: 8.368193060 seconds
Execution time for 104 core(s) [52 from each socket]: 8.390424942 seconds
Execution time for 106 core(s) [53 from each socket]: 8.411903637 seconds
Execution time for 108 core(s) [54 from each socket]: 8.407294008 seconds
Execution time for 110 core(s) [55 from each socket]: 8.451436687 seconds
Execution time for 112 core(s) [56 from each socket]: 8.586011853 seconds
"""



cores = []
times = []
for line in data.strip().split("\n"):
    core_str, time_str = line.split(":")
    core = int(core_str.split()[3])  # extract the number of cores
    time = float(time_str.split()[0])  # extract the execution time
    cores.append(core)
    times.append(time)


fig, ax = plt.subplots(figsize=(10, 6))
ax.plot(cores, times, '-o', label="Execution time (s)", color='blue', markersize=8, markeredgecolor='black')

# Title and labels
ax.set_title('Execution time for 2 to 112 cores : each pair is splitted between both sockets', fontdict={'size': 16})
ax.set_xlabel('Number of core (same socket)', fontsize=14)
ax.set_ylabel('Time (second)', fontsize=14)
ax.tick_params(labelsize=12)
ax.axvline(x=56, color='red', linestyle='--', label='56 cores == 1 socket')
ax.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
ax.legend(fontsize=10)
plt.tight_layout()
plt.show()
