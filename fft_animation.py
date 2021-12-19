import os
import struct
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

filename = "C:/Users/natha/fftf"
plt.rcParams['figure.figsize'] = [13, 7]

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
(lines,) = ax.plot(x_values, double_values, animated=True)
plt.show(block=False)
plt.pause(0.1)
bg = fig.canvas.copy_from_bbox(fig.bbox)
ax.draw_artist(lines)
fig.canvas.blit(fig.bbox)
try:
    while 1:
        stamp = os.stat(filename).st_mtime
        while(stamp == cached_time):
            stamp = os.stat(filename).st_mtime
        cached_time = os.stat(filename).st_mtime
        with open(filename, mode='rb') as file: # b is important -> binary
            fileContent = file.read()
        double_values = struct.unpack('=2049d', fileContent)
        fig.canvas.restore_region(bg)
        lines.set_ydata(double_values)
        ax.draw_artist(lines)
        fig.canvas.blit(fig.bbox)
        fig.canvas.flush_events()
except KeyboardInterrupt:
    exit(1)