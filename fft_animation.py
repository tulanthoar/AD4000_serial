import os
import struct
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

plt.style.use('fivethirtyeight')
filename = "C:/Users/natha/plotf"

x_values = range(1,2050)
y_values = []
fig, ax = plt.subplots()
ax.axis([1, 2050, -100, 100])
cached_time = os.stat(filename).st_mtime
stamp = os.stat(filename).st_mtime
while(stamp == cached_time):
    stamp = os.stat(filename).st_mtime
cached_time = os.stat(filename).st_mtime
with open(filename, mode='rb') as file: # b is important -> binary
    fileContent = file.read()
double_values = struct.unpack('=2049d', fileContent)
lines = ax.plot(double_values)

fig.canvas.manager.show() 
try:
    while 1:
        stamp = os.stat(filename).st_mtime
        while(stamp == cached_time):
            stamp = os.stat(filename).st_mtime
        cached_time = os.stat(filename).st_mtime
        with open(filename, mode='rb') as file: # b is important -> binary
            fileContent = file.read()
        double_values = struct.unpack('=2049d', fileContent)
        lines[0].set_ydata(double_values)
        fig.canvas.draw()
        fig.canvas.flush_events()
except KeyboardInterrupt:
    exit(1)