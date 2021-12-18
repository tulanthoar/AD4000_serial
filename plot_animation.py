# python_live_plot.py

import random
import os
import struct
from itertools import count
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

plt.style.use('fivethirtyeight')
filename = "C:/Users/natha/plotf"

x_values = range(1,2049)
y_values = []
fig, ax = plt.subplots()
ax.axis([1, 2049, 0, 3.3])
cached_time = os.stat(filename).st_mtime
stamp = os.stat(filename).st_mtime
while(stamp == cached_time):
    stamp = os.stat(filename).st_mtime
cached_time = os.stat(filename).st_mtime
# stamp = os.stat(filename).st_mtime
# while(stamp == cached_time):
#     stamp = os.stat(filename).st_mtime
# cached_time = os.stat(filename).st_mtime
with open(filename, mode='rb') as file: # b is important -> binary
    fileContent = file.read()
y_values = struct.unpack('=2048H', fileContent)
double_values = np.array(y_values) * 3.3 / 3500
lines = ax.plot(double_values)

fig.canvas.manager.show() 
try:
    while 1:
        stamp = os.stat(filename).st_mtime
        while(stamp == cached_time):
            stamp = os.stat(filename).st_mtime
        cached_time = os.stat(filename).st_mtime
        # stamp = os.stat(filename).st_mtime
        # while(stamp == cached_time):
        #     stamp = os.stat(filename).st_mtime
        # cached_time = os.stat(filename).st_mtime
        with open(filename, mode='rb') as file: # b is important -> binary
            fileContent = file.read()
        y_values = struct.unpack('=2048H', fileContent)
        double_values = np.array(y_values) * 3.3 / 3500
        lines[0].set_ydata(double_values)
        fig.canvas.draw()
        fig.canvas.flush_events()
except KeyboardInterrupt:
    exit(1)