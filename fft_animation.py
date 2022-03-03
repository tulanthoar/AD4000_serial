import time
import os
import sys
import struct
import matplotlib.pyplot as plt
import numpy as np
import tkinter

# name of file given to serial_fft.exe
filename = sys.argv[1]
plt.rcParams['figure.figsize'] = [10, 5.5]

# x values range from 0 to approximately 300 kHz
x_values = np.array(range(0,8193)) * 0.14258 / 4 * float(sys.argv[2]) / 300
y_values = []
# create the figure and axis objects
fig, ax = plt.subplots()
# set the axis limits
ax.axis([x_values[0], x_values[-1], -140, 5])
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
float_values = struct.unpack('=8193f', fileContent)
# plot the initial data
(lines,) = ax.plot(x_values, float_values, animated=True, linewidth=0.5, marker='x', markeredgecolor='red', markersize=4)
plt.xlabel("Frequency [kHz]")
plt.ylabel("Magnitude [dB]")
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
# flush all the events to update the image on screen
fig.canvas.flush_events()
# start the timer
start = time.time()
# start counting frames at 0
frames = 0

# open the binary file, b is important -> binary
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
            # start at the beginning of the file
            file.seek(0)
            # read the binary data
            fileContent = file.read()
            # unpack the y values as doubles
            float_values = struct.unpack('=8193f', fileContent)
            # restore the canvas to the cached background image
            fig.canvas.restore_region(bg)
            # set the data to the lines
            lines.set_ydata(float_values)
            # draw the lines
            ax.draw_artist(lines)
            # update the canvas
            fig.canvas.blit(fig.bbox)
            # flush all the events to update the image on screen
            fig.canvas.flush_events()
            # increment the frame counter to keep track of average frame time
            frames += 1
    except KeyboardInterrupt:
        elapsed = (time.time() - start)
        frameTime = elapsed / frames
        throughput = 16384 / frameTime
        print(f"average frame time {frameTime * 1000} ms")
        print(f"average throughput {throughput} sps")
        exit(1)
    except tkinter.TclError:
        elapsed = (time.time() - start)
        frameTime = elapsed / frames
        throughput = 16384 / frameTime
        print(f"average frame time {frameTime * 1000} ms")
        print(f"average throughput {throughput} sps")
        exit(1)
