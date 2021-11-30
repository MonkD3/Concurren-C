#!usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams
import argparse
import csv
import os


# Change the base settings of the plots
rcParams['axes.grid'] = True # Put a grid on the graphs
rcParams['figure.figsize'] = [12,8] # Make the figure bigger

# Parse arguments here (easier to deal with multiple plots and data files)
parser = argparse.ArgumentParser();
parser.add_argument('--datapath', type=str, nargs="+", help='Give a list of the paths to your data files here. (.csv)')
parser.add_argument('--legend', type=str, nargs="+", help="Give here a list of the legends for each data file")
parser.add_argument('--graphpath', type=str, help='Give the path for where you want your graph to be.') 
args = parser.parse_args()

n_files = len(args.datapath)
mesures = [None for _ in range(n_files)]
# Read the csv and collect the header and measures

for idx, data_file in enumerate(args.datapath) :
    if (os.path.exists(data_file)):
        with open(data_file, "r") as file:
            reader = iter(csv.reader(file)) # Create an iterator on the file
            header = reader.__next__() # Read the first line of the file
            mesures[idx] = np.array([np.array(row, dtype=float) for row in reader]) # Read the whole file
    else :
        print("Skipping the file {} as it appears it doesn't exist.", data_file)
        n_files -= 1
        del mesures[idx]
        del args.legend[idx]

mesures = np.array(mesures)

ax = plt.subplot()

# Compute useful data
mean = np.mean(mesures, axis=1)
std = np.std(mesures, axis=1)

ax.set_ylabel("Temps [s]")
ax.set_xlabel("Nombre de Threads")

ax.set_xticks(range(len(header)))
ax.set_xticklabels(header)
ax.set_ylim(bottom=0, top = 1.1*(mean.max() + std.max()))

# Plot the graph of means
ax.set_title("Temps moyen pour n threads")
for legend, mes, stdd in zip(args.legend, mean, std):
    ax.errorbar(range(len(header)), mes, yerr=stdd, fmt='.-', capsize=3, label=legend)
ax.legend()

# Save the figure in pdf
plt.savefig(args.graphpath + ".pdf")
print("Your graph has been saved to {}.".format(args.graphpath))
