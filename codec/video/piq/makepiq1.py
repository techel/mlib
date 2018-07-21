import glob, sys, struct

def main(argv):

	if len(argv) < 2:
		print("Description: Creates a PIQ (picture sequence)-container from multiple input files.")
		print("Usage: <searchpath> <output file>")
		print("<searchpath> may contain wildcards.");
		return

	inpath = argv[0]
	outfilename = argv[1]
	
	files = glob.glob(inpath);
	files.sort();

	idx = 0
	with open(outfilename, "wb") as outfile:
		outfile.write(b"PIQ!\x01") #signature and version
		outfile.write(struct.pack("<i", len(files))) #number of images
		
		for i in files:
			with open(i, "rb") as infile:
				all = infile.read()
				print("{}: {} (length: {})".format(idx, i, len(all)))
				
				outfile.write(struct.pack("<i", len(all))) #chunk length
				outfile.write(all) #chunk data
				
				idx += 1
		
	print("{} files -> {}".format(idx, outfilename))
		
if __name__ == "__main__":
   main(sys.argv[1:])