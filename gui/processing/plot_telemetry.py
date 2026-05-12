import matplotlib.pyplot as plt 
import os
import csv
from pathlib import Path
import glob

time = []
depth = []
ref_depth = []
control_vol = []
piston_vol = []

list_of_files = glob.glob("/Users/alfieeagleton/Desktop/Uni/Thesis/Code/gui/logs/telemetry/*.csv") 
latest_file = max(list_of_files, key=os.path.getctime)

with open(latest_file ,'r') as csvfile:
    plots = csv.reader(csvfile, delimiter = ',')
    
    for row in plots:
        time.append(round(float(row[0]), 1))
        depth.append(float(row[1]))
        ref_depth.append(float(row[2]))
        control_vol.append(float(row[3]))
        piston_vol.append(float(row[4]))

plt.figure()
plt.plot(time, depth, color = 'c', label = "Depth",)
plt.plot(time, ref_depth, color = 'k', label = "Reference Depth",)
plt.xlabel('Time (s)')
plt.ylabel('Depth (m)')
plt.title('Reference Tracking')
plt.legend()
plt.show()

plt.figure()
plt.plot(time, control_vol, color = 'b', label = "Control Volume")
plt.plot(time, piston_vol, color = 'r', label = "Piston Volume")
plt.xlabel('Time (s)')
plt.ylabel('Volume (mL)')
plt.title('Piston Position')
plt.legend()
plt.show()
