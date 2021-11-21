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
parser.add_argument('--title', type=str, help='Give the title of the plot here')
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

# create the subplots, the first for the boxplot, second and third for mean,std
ax = [plt.subplot(212), plt.subplot(211)]

# Compute useful data
mean = np.mean(mesures, axis=1)
std = np.std(mesures, axis=1)
lower_confidence = mean - 2*std 
upper_confidence = mean + 2*std 

# Common set up to all :
for axe in ax :
    axe.set_xticks(range(len(header)))
    axe.set_xticklabels(header)
    axe.set_ylabel("Temps [s]")
    axe.set_xlabel("Nombre de Threads")

# Plot the graph of means
ax[0].set_title("Temps moyen pour n threads")
for legend, mes in zip(args.legend, mean):
    ax[0].errorbar(range(len(header)), mes, yerr=std, fmt='.-', label=legend)

ax[0].legend()

# PLot the graph of std
ax[1].set_title("Ecart-type moyen pour n threads")
for legend, mes in zip(args.legend, std):
    ax[1].scatter(range(len(header)), mes, label=legend)
ax[1].legend()

# Give a title to the figure
plt.suptitle(args.title)
# Save the figure in pdf
plt.savefig(args.graphpath + ".pdf")
print("Your graph has been saved to {}.".format(args.graphpath))
