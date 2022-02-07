import time
import os
import struct
import matplotlib.pyplot as plt
import numpy as np
import tkinter

filename = "C:/Users/natha/plotf"
# default figure size
plt.rcParams['figure.figsize'] = [11, 6]

# scaling factor for a 12 bit converter with 3.3V range
scale = 3.3 / 65536
# there are 4096 equally spaced points, time in ms
x_values = np.array(range(0,4096)) * 4.388571428571429e-04
y_values = []
# create the figure and axis objects
fig, ax = plt.subplots()
# set the axis limits
ax.axis([0, 4096 * 4.388571428571429e-04, 0, 3.3])
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
y_values = struct.unpack('=4096H', fileContent)
# convert the integer values to their corresponding voltages
double_values = np.array(y_values) * scale
# plot the initial data
(lines,) = ax.plot(x_values, double_values, animated=True)
plt.xlabel("Time [ms]")
plt.ylabel("Voltage [V]")
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
# start the timer
start = time.time()
# start counting frames at 0
frames = 0

with open(filename, mode='rb') as file:
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
            y_values = struct.unpack('=4096H', fileContent)
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
            frames += 1
    except KeyboardInterrupt:
        elapsed = (time.time() - start)
        frameTime = elapsed / frames
        throughput = 4096 / frameTime
        print(f"average frame time {frameTime * 1000} ms")
        print(f"average throughput {throughput} sps")
        exit(1)
    except tkinter.TclError:
        elapsed = (time.time() - start)
        frameTime = elapsed / frames
        throughput = 4096 / frameTime
        print(f"average frame time {frameTime * 1000} ms")
        print(f"average throughput {throughput} sps")
        exit(1)