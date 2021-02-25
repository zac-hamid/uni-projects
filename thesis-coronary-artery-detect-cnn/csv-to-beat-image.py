import matplotlib.pyplot as plt
import numpy as np
import os
from os import walk
import cv2
import csv
import math
import time

import biosppy.signals.ecg as bioecg

data_dir = "data/"
image_dir = "data/images/"

num_leads = 12

# Function to plot on a subplot with necessary parameters
def plot(x,y,i):
	plt.subplot(num_leads,1,i)
	plt.plot(x,y, 'k', linewidth=0.5)
	plt.xticks([]), plt.yticks([])
	# Stop lines connected axis tick marks being shown
	for s in plt.gca().spines.values():
		s.set_visible(False)
	return

# For all directories that aren't images (that is, all separate .csv database directories)
for (dirpath, dirnames, filenames) in walk(data_dir):
	if (dirpath.split("/")[1].split("\\")[0] != "images"):
		if filenames:
			for file in filenames:
				if file==".DS_Store":
					continue
				# Open each .csv file within a given directory and process it

				print("opening ", file)
				start = time.time()
				reader = csv.reader(open(dirpath + "/" + file, newline=''), delimiter=',')
				x = list(reader)
				matrix = np.array(x).astype("float")

				# Use Christov Segmentation method on 6th lead of ECG data (Lead I) 
				# Since this is the most stable lead (by inspection) for the R peak
				r_peaks = bioecg.christov_segmenter(matrix[5,:])

				# Using the detected R-peaks of the signal, segment the ECG signal into individual beats 
				# by taking the RR interval from previous beat and dividing by 2
				big_matrix = []
				for i in range(len(r_peaks[0])-1):
					start = 0
					end = 0
					if (i != 0):
						start = (r_peaks[0][i] - ((r_peaks[0][i] - r_peaks[0][i-1])/2)).astype("int")
					if (i == (len(r_peaks[0]) - 1)):
						big_matrix.append(matrix[:,start:])
					else:
						end = (r_peaks[0][i] + ((r_peaks[0][i+1] - r_peaks[0][i])/2)).astype("int")
						big_matrix.append(matrix[:,start:end])
				big_matrix.append(matrix[:,end:])
				print(time.time() - start, "s to segment beats")
				# frameon=False stops the figure frame from being drawn
				fig = plt.figure(frameon=False)

				# Set to an appropriate size for displaying the ECG data
				fig.set_figheight(20)
				fig.set_figwidth(3)

				plt.subplots_adjust(0,0,1,1,0,0)
				
				# Create a directory for the patient if it doesn't already exist
				dir_string = dirpath.split("/")[1]
				filename = file.split(".")[0]
				if not os.path.exists(image_dir + dir_string + "/" + filename):
					os.mkdir(image_dir + dir_string + "/" + filename)
				# Plot all beats and process/save as their own files within patient directory
				for i in range(len(r_peaks[0])):
					if (i > 120):
						break
					print("printing beat: ", i)
					if os.path.exists(image_dir + dir_string + "/" + filename + "/" + filename + "_beat" + str(i) + ".png"):
						continue
					for j in range(num_leads):
						plot(big_matrix[i][0],big_matrix[i][j+1],j+1)
					# Use matplotlib to save the figure that was plotted to a png file
					fig.savefig(image_dir + dir_string + "/" + filename + "/" + filename + "_beat" + str(i) + ".png", dpi=500, bbox_inches='tight')
					# Read the file into OpenCV
					image = cv2.imread(image_dir + dir_string + "/" + filename + "/" + filename + "_beat" + str(i) + ".png")
					
					# Crop image to remove white space on the left and right edges and convert to grayscale
					if (image.shape[1] >= 1600):
						cropped_image = image[:,115:-115]
						gray_image = cv2.cvtColor(cropped_image, cv2.COLOR_BGR2GRAY)
					else:
						gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

					# Down-sample image - 25% of width and maintaining aspect ratio for the height so the image doesn't get squished
					(height, width) = gray_image.shape
					resized = cv2.resize(gray_image,(math.floor(width/4), math.floor(math.floor(width/4)/(width/height))))

					# Write the processed file back
					cv2.imwrite(image_dir + dir_string + "/" + filename + "/" + filename + "_beat" + str(i) + ".png", resized)
					plt.clf()
					print(time.time() - start, "s to process beat ", i)
				plt.close()
				end = time.time()
				print("took ", end-start, "s to process all beats in .csv file!")