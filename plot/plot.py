import matplotlib.pyplot as plt
import sys

# call it with 'python plot path/to/directory simulation_name'
def main(directory, simulation_name):

	content = 0
	with open(directory + "/" + simulation_name, 'r') as f:
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
	plt.show()



main(sys.argv[1], sys.argv[2])

# plt.plot([1, 2, 3, 4], [2, 5, 3, 1])
# plt.ylabel('some numbers')
# plt.show()