#!/usr/bin/env python
import keras
from keras.models import Sequential
from keras.layers import *
from keras import regularizers
from keras.callbacks import *
from keras import backend as K
from keras.utils import multi_gpu_model
import numpy as np
from os import walk
from sklearn.model_selection import train_test_split
import matplotlib.image as mpimg

data_dir = "data/"
stable_dir = "data/stable"
unstable_dir = "data/unstable"
img_width = 184
img_height = 904

train_size_percent = 0.9
val_split = 0.2 # % of training data to split into validation data

KERNEL = (3, 3)

# Higher batch size = less training time = get results faster
BATCH_SIZE = 32
EPOCHS = 1000

# Stable = 0
# Unstable = 1

# Callback for calculating sensitivity of prediction
def sensitivity(y_true, y_pred):
    true_positives = K.sum(K.round(K.clip(y_true * y_pred, 0, 1)))
    possible_positives = K.sum(K.round(K.clip(y_true, 0, 1)))
    return true_positives / (possible_positives + K.epsilon())

# Callback for calculating specificity of prediction
def specificity(y_true, y_pred):
    true_negatives = K.sum(K.round(K.clip((1-y_true) * (1-y_pred), 0, 1)))
    possible_negatives = K.sum(K.round(K.clip(1-y_true, 0, 1)))
    return true_negatives / (possible_negatives + K.epsilon())

# Callback for monitoring test accuracy over the epochs
class testing_callback(keras.callbacks.Callback):
	def __init__(self, test_data):
		self.test_data = test_data
		self.test_history = np.empty((0, 5))

	def on_epoch_end(self, epoch, logs={}):
		x, y = self.test_data
		loss, acc, sens, spec = self.model.evaluate(x, y, verbose=0)
		self.test_history = np.append(self.test_history, np.array([[epoch, loss, acc, sens, spec]]), axis=0)
		filename = 'test_history.txt'

		if os.path.exists(filename):
			append_write = 'a' # append if already exists
		else:
			append_write = 'w' # make a new file if not

		hist = open(filename, append_write)
		hist.write('[{}, {}, {}, {}, {}]\n'.format(epoch, loss, acc, sens, spec))
		hist.close()
		print('\nTesting loss: {}, acc: {}, sensitivity: {}, specificity: {}\n'.format(loss, acc, sens, spec))

# Callback for monitoring training accuracy over the batches
class per_batch_accuracy(keras.callbacks.Callback):
	def __init__(self):
		self.acc = []

	def on_batch_end(self, batch, logs={}):
		filename = 'accuracy_hist.txt'

		if os.path.exists(filename):
			append_write = 'a' # append if already exists
		else:
			append_write = 'w' # make a new file if not

		hist = open(filename, append_write)
		hist.write('[{}, {}, {}, {}, {}]\n'.format(batch, logs.get('loss'), logs.get('acc'), logs.get('sensitivity'), logs.get('specificity')))
		hist.close()
		print('Training loss: {}, acc: {}, sens: {}, spec: {}\n'.format(logs.get('loss'), logs.get('acc'), logs.get('sensitivity'), logs.get('specificity')))


# This function will get training and testing data
def get_data():
	X = []
	Y = np.array([])
	i = 0
	print("Getting data...")
	# Search directories to find stable/unstable data and put them into an array
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
				# Values are [0,1] by default
				img = mpimg.imread(dirpath + "/" + file)
				print("Got file: ", file)
				# X is a list of np.array objects
				if (dirpath == stable_dir):
					# All filenames in here are stable images
					Y = np.append(Y, 0)
					X.append(np.array(img))
				elif (dirpath == unstable_dir):
					# All filenames in here are unstable images
					Y = np.append(Y, 1)
					X.append(np.array(img))
	print("Running dstack...")
	# Make X a 3D array (depth is each image)
	X = np.dstack(X)
	print("After d stack")
	X_new = X.transpose(2,0,1)

	x_train, x_test, y_train, y_test = train_test_split(X_new, Y, test_size = 1 - train_size_percent, train_size = train_size_percent)

	# CNN needs the layer to be (number of images, height, width, 1)
	x_train = np.expand_dims(x_train, axis=3)
	x_test = np.expand_dims(x_test, axis=3)
	print("Got all data!")
	# X = array of images, depth is how you select the image
	# Y = classification of image [0,1]
	return x_train, x_test, y_train, y_test


def build_model():
	cnn = Sequential()
	cnn.add(Conv2D(32, KERNEL, input_shape = (img_height, img_width, 1), kernel_initializer='glorot_uniform'))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())

	cnn.add(Conv2D(32, KERNEL, strides=(1, 1), kernel_initializer='glorot_uniform'))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())

	cnn.add(MaxPool2D(pool_size=(2, 2), strides=(2, 2)))

	cnn.add(Conv2D(64, KERNEL, strides=(1, 1), kernel_initializer='glorot_uniform'))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())

	cnn.add(Conv2D(64, KERNEL, strides=(1, 1), kernel_initializer='glorot_uniform'))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())

	cnn.add(MaxPool2D(pool_size=(2, 2), strides=(2, 2)))

	cnn.add(Conv2D(128, KERNEL, strides=(1, 1), kernel_initializer='glorot_uniform'))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())

	cnn.add(Conv2D(128, KERNEL, strides=(1, 1), kernel_initializer='glorot_uniform'))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())

	cnn.add(MaxPool2D(pool_size=(2, 2), strides=(2, 2)))

	cnn.add(Flatten())

	cnn.add(Dense(256, kernel_regularizer=regularizers.l2(0.0005)))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())
	cnn.add(Dropout(0.4))

	cnn.add(Dense(256, kernel_regularizer=regularizers.l2(0.0005)))
	cnn.add(keras.layers.LeakyReLU(alpha=0.1))
	cnn.add(BatchNormalization())
	cnn.add(Dropout(0.4))

	cnn.add(Dense(1, activation='sigmoid'))

	return cnn

print("Building model...")
model = build_model()

# parallel_model = multi_gpu_model(model, gpus=4)

# print("Compiling optimizer...")
# # optimizer can change to rmsprop
# parallel_model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy', sensitivity, specificity])
# parallel_model.summary()
model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy', sensitivity, specificity])
model.summary()

# UNCOMMENT BELOW FOR TRAINING AND TESTING OF NETWORK
# print("Getting data...")
# X_train, X_test, Y_train, Y_test = get_data()

# csv_log_cb = CSVLogger("training.csv", append=True)
# training_acc_cb = per_batch_accuracy()

# early_stop_cb = EarlyStopping(monitor='val_loss', min_delta=0, patience=200, verbose=1, mode='min', baseline=None, restore_best_weights=False)

# model_save_filepath = "weights-best.hdf5"
# model_save_cb = ModelCheckpoint(model_save_filepath, monitor='val_loss', verbose=1, save_best_only=True, mode='min')
# test_hist = testing_callback((X_test, Y_test))

# # VERBOSE = 2 IS FOR 1 PROGRESS BAR PER EPOCH, 1 MEANS IT WILL SHOW PROGRESS PER BATCH
# model_hist = parallel_model.fit(X_train, Y_train, batch_size=BATCH_SIZE, epochs=EPOCHS, validation_split=val_split, verbose=1, shuffle=True, callbacks=[training_acc_cb, model_save_cb, csv_log_cb, test_hist, early_stop_cb])
# print(model_hist.history)

# print("Testing history: ")
# print(test_hist.test_history)
