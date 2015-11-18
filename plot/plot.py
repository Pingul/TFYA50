import matplotlib.pyplot as plt
import sys

def plot_mdf(file_path):
	content = 0
	with open(file_path, 'r') as f:
		content = [line.rstrip("\n") for line in f.readlines()]
	
	# print(dir(content))
	timestamps = []
	values = []
	for line in content:
		res = line.split("\t")
		# print(res[1])
		timestamps.append(float(res[0]))
		values.append(float(res[1]))

	plt.plot(timestamps, values)
	plt.ylabel("values")
	plt.xlabel("time")
	plt.title(file_path)

# call it with 'python plot path/to/directory simulation_name'
def main(directory, simulation_name):

	if simulation_name.split(".")[-1] == "mdf": # if we run with a single mdf-file, we only plot that one
		plot_mdf(directory + "/" + simulation_name)
		plt.show()
		return
	elif simulation_name.split(".")[-1] != "sim": # if we havn't written the file ending, append ".sim"
		simulation_name += ".sim"

	files = []
	with open(directory + "/" + simulation_name, 'r') as f:
		files = [line.rstrip("\n") for line in f.readlines()]
	
	figures = 0
	for file in files:
		figures += 1
		plt.figure(figures)
		plot_mdf(directory + "/" + file)
	plt.show()

main(sys.argv[1], sys.argv[2])