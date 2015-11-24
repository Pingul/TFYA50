import matplotlib.pyplot as plt

def main():
	files = ["4_vlist_10_totalEnergy.mdf", "4_vlist_10_kinetic.mdf", "4_vlist_10_potential.mdf"]

	data = []
	for file_name in files:
		file_path = "data"
		content = []
		with open(file_path + "/" + file_name, 'r') as f:
			content = [line.rstrip("\n") for line in f.readlines()]
	
		timestamps = []
		values = []
		for line in content:
			res = line.split("\t")
			timestamps.append(float(res[0]))
			values.append(float(res[1]))

		avg = sum(values[100:])/(len(values) - 100)
		d = {"file" : file_name, "timestamps" : timestamps, "values" : values, "avg" : avg}
		data.append(d)


	for d in data:
		for index in range(0, len(d["values"])):
			d["values"][index] -= d["avg"]

	plt.plot(data[0]["timestamps"], data[0]["values"], "r", data[1]["timestamps"], data[1]["values"], "g", data[2]["timestamps"], data[2]["values"], "b")
	# plt.plot(data[0]["timestamps"], data[0]["values"], "r", data[1]["timestamps"], data[1]["values"], "g")
	plt.show()

main()
