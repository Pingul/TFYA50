import os
import sys
import time

def main(file):
	files = []
	with open(file, 'r') as f:
		files = [line.rsplit("#")[0].rstrip("\n") for line in f.readlines() if line.rsplit("#")[0].rstrip("\n") != ""]

	t = time.time()
	bin = "./main "
	if os.name == "nt":
		bin = r"MDSim\x64\Release\MDSim.exe "
	for f in files:
		print("RUNNING '" + f + "'\n")
		call = bin + f
		print(call)
		os.system(call)
		current_t = time.time()
		print("\nCOMPLETED '" + f + "'\nCURRENTLY SPENT " + str(current_t - t) + " SECONDS EXECUTING")

main(sys.argv[1])
