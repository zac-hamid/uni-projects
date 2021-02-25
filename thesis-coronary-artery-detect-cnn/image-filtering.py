#!/usr/bin/env python
import numpy as np
import cv2
from os import walk
import os

data_dir = "data/"
stable_dir = "data/stable"
unstable_dir = "data/unstable"

print("Getting data...")
for (dirpath, dirnames, filenames) in walk(data_dir):
	# Are there files that aren't directories inside dirpath?
	if filenames:
		# For each file in this current dirpath
		for file in filenames:
			# Just in case
			if file==".DS_Store":
				continue
			# Want to make sure the file we're reading is a .png file
			if file.split('.')[1] != "png":
				continue
			# Dont want to recursively filter
			if (dirpath.split('\\')[-1] == "filtered"):
				continue
			if (os.path.exists(dirpath + "/filtered/" + file)):
				continue
			print("reading file: ", dirpath + "/" + file)
			img = cv2.imread(dirpath + "/" + file, cv2.IMREAD_GRAYSCALE)
			# Discovered that the size can be reduced further using INTER_AREA interpolation
			img_resized = cv2.resize(img, (180,900), interpolation=cv2.INTER_AREA)
			# Padded image 184x904
			img_padded = cv2.copyMakeBorder(img_resized, 2, 2, 2, 2, cv2.BORDER_CONSTANT, value=[255, 255, 255])
			# The size reduction blurs the image quite subtantially so a sharpening kernel is applied to the image to improve the quality
			kernel = np.array([[-1, -1, -1], [-1, 9, -1], [-1, -1, -1]])
			img_filtered = cv2.filter2D(img_padded, -1, kernel)

			if not os.path.exists(dirpath + "/filtered/"):
				os.makedirs(dirpath + "/filtered/")
			print("writing file to: ", dirpath + "/filtered/" + file)
			cv2.imwrite(dirpath + "/filtered/" + file, img_filtered)
