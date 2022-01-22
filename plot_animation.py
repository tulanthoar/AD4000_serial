import time
import os
import struct
import matplotlib.pyplot as plt
import numpy as np

filename = "C:/Users/natha/plotf"
# default figure size
plt.rcParams['figure.figsize'] = [13, 7]

# scaling factor for a 12 bit converter with 3.3V range
scale = 3.3 / 4096
# there are 2048 equally spaced points
x_values = range(1,2049)
y_values = []
# create the figure and axis objects
fig, ax = plt.subplots()
# set the axis limits
ax.axis([1, 2049, 0, 3.3])
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
# unpack the y values, which are 16 bit unsigned integers
y_values = struct.unpack('=2048H', fileContent)
# convert the integer values to their corresponding voltages
double_values = np.array(y_values) * scale
# plot the initial data
(lines,) = ax.plot(x_values, double_values, animated=True)
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
        # unpack the y values as unsigned 16 bit integers
        y_values = struct.unpack('=2048H', fileContent)
        # scale the double values to their corresponding voltages
        double_values = np.array(y_values) * scale
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