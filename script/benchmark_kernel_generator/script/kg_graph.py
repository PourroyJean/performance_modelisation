from pathlib import Path
import multiprocessing
import subprocess
import sys
import os

import matplotlib.pyplot as plt


class RunResult:
    def __init__(self, flops_single: float, flops_double: float, bench_freq: float) -> None:
        self.flops_single = flops_single
        self.flops_double = flops_double
        self.bench_freq = bench_freq


def run(*, threads: int, width: int) -> RunResult:

    kg_path = Path(__file__).parent / "kg"
    kg_env = os.environ.copy()
    kg_env["OMP_PLACES"] = "threads"
    kg_env["OMP_PROC_BIND"] = "true"
    kg_env["OMP_NUM_THREADS"] = str(threads)

    kg_proc = subprocess.run([kg_path, *sys.argv[1:], "-M", "true", "-W", str(width)], env=kg_env, stdout=subprocess.PIPE)
    kg_output = kg_proc.stdout.decode()

    flops_single = 0
    flops_double = 0
    bench_freq = 0

    precision = False
    for line in kg_output.splitlines():

        line = line.strip()

        if line.startswith("+ Bench frequency is "):
            bench_freq = float(line[21:].replace("GHz", ""))
            continue

        values = list(filter(lambda s: len(s), line.split(" ")))
        if not len(values):
            continue

        if values[0] == "PRECISION":
            precision = True
        elif precision:
            if values[0] == "Single":
                flops_single = float(values[2])
            elif values[0] == "Double":
                flops_double = float(values[2])
            else:
                precision = False

    return RunResult(flops_single, flops_double, bench_freq)


def main():

    cpu_count = len(os.sched_getaffinity(0))

    threads = []
    flops_128 = []
    freq_128 = []
    flops_256 = []
    freq_256 = []
    flops_512 = []
    freq_512 = []

    for i in range(1, cpu_count + 1):

        threads.append(i)

        print(f"threads={i}, width=128")
        res = run(threads=i, width=128)
        flops_128.append(res.flops_double)
        freq_128.append(res.bench_freq)

        print(f"threads={i}, width=256")
        res = run(threads=i, width=256)
        flops_256.append(res.flops_double)
        freq_256.append(res.bench_freq)

        print(f"threads={i}, width=512")
        res = run(threads=i, width=512)
        flops_512.append(res.flops_double)
        freq_512.append(res.bench_freq)

    fig, axs = plt.subplots(2)

    ax0, ax1 = axs

    ax0.plot(threads, flops_128, "-g", label="AVX")
    ax0.plot(threads, flops_256, "-r", label="AVX2")
    ax0.plot(threads, flops_512, "-b", label="AVX-512")
    ax0.legend()
    ax0.set_ylabel("FLOPs")
    ax0.grid()

    ax1.plot(threads, freq_128, "-g", label="AVX")
    ax1.plot(threads, freq_256, "-r", label="AVX2")
    ax1.plot(threads, freq_512, "-b", label="AVX-512")
    ax1.legend()
    ax1.set_ylabel("GHz")
    ax1.set_xlabel("Threads")
    ax1.grid()
    
    plt.savefig("plot.png")


if __name__ == "__main__":
    main()
