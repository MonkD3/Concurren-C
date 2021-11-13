#!usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams
import argparse
import csv
import platform

platform.processor()


# Change the base settings of the plots
rcParams['axes.grid'] = True # Put a grid on the graphs
rcParams['figure.figsize'] = [12,8] # Make the figure bigger

# Parse arguments here (easier to deal with multiple plots and data files)
parser = argparse.ArgumentParser();
parser.add_argument('--datapath', type=str, help='Give the path to your data file here. (.csv)')
parser.add_argument('--graphpath', type=str, help='Give the path for where you want your graphs to be.') 
parser.add_argument('--title', type=str, help='Give the title of the plot here')
args = parser.parse_args()


# Read the csv and collect the header and measures
with open(args.datapath, "r") as file:
    reader = iter(csv.reader(file)) # Create an iterator on the file
    header = reader.__next__() # Read the first line of the file
    mesures = np.array([np.array(row, dtype=float) for row in reader][:-1]) # Read the whole file


# create the subplots, the first for the boxplot, second and third for mean,std
ax = [plt.subplot(212), plt.subplot(221), plt.subplot(222)]

# Compute useful data
mean = np.mean(mesures, axis=0)
std = np.std(mesures, axis=0)
lower_confidence = mean - 2*std 
upper_confidence = mean + 2*std 

# Common set up to all :
for axe in ax :
    axe.set_xticks(range(len(header)))
    axe.set_xticklabels(header)
    axe.set_ylabel("Temps [s]")
    axe.set_xlabel("Nombre de Threads")

# Make a boxplot
ax[0].set_title("Boxplot")
ax[0].boxplot(mesures)
ax[0].set_xticks(range(1, len(header) + 1)) # Correct the ticks
ax[0].set_xticklabels(header)


# Plot the graph of means
ax[1].set_title("Temps moyen pour n threads")
ax[1].scatter(range(len(header)), mean, label="Moyenne observée")
ax[1].fill_between(range(len(header)), lower_confidence, upper_confidence, label="Intervalle de confiance à 95%", alpha = 0.2)
ax[1].legend()

# PLot the graph of std
ax[2].set_title("Ecart-type moyen pour n threads")
ax[2].scatter(range(len(header)), np.std(mesures, axis=0))

# Give a title to the figure
plt.suptitle(args.title)
# Save the figure in png and pdf
plt.savefig(args.graphpath)
plt.savefig(args.graphpath)
print("Your graph has been saved to {}.".format(args.graphpath))
