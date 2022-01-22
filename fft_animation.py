import time
import os
import struct
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

filename = "C:/Users/natha/fftf"
plt.rcParams['figure.figsize'] = [13, 7]

# x values range from 0 to approximately 300 kHz
x_values = np.array(range(0,8193)) * 0.14258 / 4
y_values = []
# create the figure and axis objects
fig, ax = plt.subplots()
# set the axis limits
ax.axis([0.143, 293, -120, 0])
# cache the time of the most recent edit of the plotting data
cached_time = os.stat(filename).st_mtime
# collect the current time of the plotting data
stamp = os.stat(filename).st_mtime
# wait while the current stamp is equal to the cached stamp
while(stamp == cached_time):
    stamp = os.stat(filename).st_mtime
# store the time stamp of the current plotting data
cached_time = os.stat(filename).st_mtime
# open the plotting data file
with open(filename, mode='rb') as file: # b is important -> binary
    # read the data
    fileContent = file.read()
# unpack the y values, which are doubles
double_values = struct.unpack('=8193d', fileContent)
# plot the initial data
(lines,) = ax.plot(x_values, double_values, animated=True, linewidth=0.5, marker='x', markeredgecolor='red', markersize=4)
# show the plot
plt.show(block=False)
# pause the plot to allow time to draw
plt.pause(0.1)
# copy the current background frame
bg = fig.canvas.copy_from_bbox(fig.bbox)
# draw the y values
ax.draw_artist(lines)
# update the canvas
fig.canvas.blit(fig.bbox)
start = time.time()

try:
    while 1:
        # collect the current time stamp
        stamp = os.stat(filename).st_mtime
        # wait while the current time stamp is equal to the cached time
        while(stamp == cached_time):
            stamp = os.stat(filename).st_mtime
        # cache the current time stamp of the data
        cached_time = os.stat(filename).st_mtime
        # open the binary file
        with open(filename, mode='rb') as file: # b is important -> binary
            # read the binary data
            fileContent = file.read()
        # unpack the y values as doubles
        double_values = struct.unpack('=8193d', fileContent)
        # restore the canvas to the cached background image
        fig.canvas.restore_region(bg)
        # set the data to the lines
        lines.set_ydata(double_values)
        # draw the lines
        ax.draw_artist(lines)
        # update the canvas
        fig.canvas.blit(fig.bbox)
        # flush all the events to update the image on screen
        fig.canvas.flush_events()
        end = time.time()
        elapsed = (end - start) * 1000
        print(f"elapsed time {elapsed} ms")
        start = end
except KeyboardInterrupt:
    exit(1)