from pathlib import Path
import subprocess
import sys
import os

import matplotlib.pyplot as plt


class RunResult:
    def __init__(self, flops_single: float, flops_double: float, freq: float, ipc: float) -> None:
        self.flops_single = flops_single
        self.flops_double = flops_double
        self.freq = freq
        self.ipc = ipc


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
    freq = 0
    ipc = 0

    precision = False
    nb_inst = False

    for line in kg_output.splitlines():

        line = line.strip()

        if line.startswith("+ Bench frequency is "):
            freq = float(line[21:].replace("GHz", ""))
            continue

        values = list(filter(lambda s: len(s), line.split(" ")))
        if not len(values):
            continue
        
        if precision:
            if values[0] == "Single":
                flops_single = float(values[2])
            elif values[0] == "Double":
                flops_double = float(values[2])
            else:
                precision = False
        elif nb_inst:
            ipc = float(values[3])
            nb_inst = False
        elif values[0] == "PRECISION":
            precision = True
        elif values[0] == "NB" and values[1] == "INSTRUCTIONS":
            nb_inst = True

    return RunResult(flops_single, flops_double, freq, ipc)


def cpu_model() -> str:
    with open("/proc/cpuinfo", "rt") as fp:
        for line in fp.readlines():
            if line.startswith("model name"):
                return line.split(": ", maxsplit=1)[1].rstrip()
    raise Exception("cannot retreive cpu model")


def main():

    cpu_count = len(os.sched_getaffinity(0))

    threads = []
    flops = {128: [], 256: [], 512: []}
    freq = {128: [], 256: [], 512: []}
    ipc = {128: [], 256: [], 512: []}

    for i in range(1, cpu_count + 1):
        threads.append(i)
        for width in (128, 256, 512):
            print(f"threads={i}, width={width}")
            res = run(threads=i, width=width)
            flops[width].append(res.flops_double)
            freq[width].append(res.freq)
            ipc[width].append(res.ipc)
    
    def plot_widths(ax, data):
        ax.plot(threads, data[128], "-g", label="AVX")
        ax.plot(threads, data[256], "-r", label="AVX2")
        ax.plot(threads, data[512], "-b", label="AVX-512")

    cpu_model_full = cpu_model()
    cpu_model_lower = cpu_model_full.lower().replace(" @ ", "_").replace(" ", "_").replace("(r)", "").replace(".", "_").replace("-", "_")

    fig, axs = plt.subplots(3)
    fig.set_size_inches((6.4, 8.0))

    ax0, ax1, ax2 = axs

    plot_widths(ax0, flops)
    ax0t = ax0.twinx()
    ax0t.plot(threads, [flop * threads for threads, flop in zip(threads, flops[128])], "--g")
    ax0t.plot(threads, [flop * threads for threads, flop in zip(threads, flops[256])], "--r")
    ax0t.plot(threads, [flop * threads for threads, flop in zip(threads, flops[512])], "--b")
    ax0.legend()
    ax0.set_ylabel("FLOPs")
    ax0t.set_ylabel("Total FLOPs")
    ax0.grid()

    fig.suptitle(f"{cpu_model_full} ({cpu_count} CPUs)")

    plot_widths(ax1, freq)
    ax1.legend()
    ax1.set_ylabel("GHz")
    ax1.grid()

    plot_widths(ax2, ipc)
    ax2.legend()
    ax2.set_ylabel("IPC")
    ax2.set_xlabel("Threads")
    ax2.grid()
    
    plt.savefig(f"kg_{cpu_model_lower}.png", dpi=400)


if __name__ == "__main__":
    main()
