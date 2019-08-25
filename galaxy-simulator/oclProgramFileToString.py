#!/usr/bin/python


import os, sys, argparse, re


def comment_remover(text):
	def replacer(match):
		s = match.group(0)
		if s.startswith('/'):
			return " " # note: a space and not an empty string
		else:
			return s

	pattern = re.compile(
		r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
		re.DOTALL | re.MULTILINE
	)

	return re.sub(pattern, replacer, text)


if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("-v", "--verbose", help="increase output verbosity", action="store_true")
	parser.add_argument("-c", "--compact", help="remove all comments and empty lines", action="store_true")
	parser.add_argument("inputFileName", help="name of input OpenCL file with source (*.cl, *.clh)", type=str)
	parser.add_argument("outputFileName", help="name of output C/C++ file with source as C char string (*.c, *.cpp, *.h)", type=str)
	args = parser.parse_args()

	if not args.inputFileName.endswith(".cl") and not args.inputFileName.endswith(".clh"):
		sys.exit("invalid input file extension: " + args.inputFileName)

	if not args.outputFileName.endswith(".c") and not args.outputFileName.endswith(".cpp") and not args.outputFileName.endswith(".h"):
		sys.exit("invalid output file extension: " + args.outputFileName)

	if args.verbose:
		print("input file: " + args.inputFileName)
		print("output file: " + args.outputFileName)

	if not args.compact:
		with open(args.inputFileName, "r") as file:
			stringSource = "\n".join([line.rstrip() for line in file]) # remove trailing whitespaces
	else:
		stringSource = open(args.inputFileName, "r").read()

	if args.verbose:
		print("\noriginal OpenCL source:")
		print(stringSource)

	if args.compact:
		stringSource = comment_remover(stringSource) # remove all comments
		stringSource = stringSource.replace("\t", " ") # replace '\t' with single space
		stringSource = stringSource.replace("   ", " ") # replace triple space with single space
		stringSource = stringSource.replace("  ", " ") # replace double space with single space

		# remove spaces around each operator and bracket
		for char in "!\"%&/()=?*+-|,.;:^{}~\'[]<>":
			stringSource = stringSource.replace(char + " ", char)
			stringSource = stringSource.replace(" " + char, char)
		
		temp = [line.strip() for line in stringSource.split("\n")] # remove leading and trailing whitespaces
		stringSource = "\n".join([line for line in temp if line]) # remove empty lines

	if not args.compact:
		stringSource = stringSource.replace("\\", "\\\\") # replace '\\' in comments
		stringSource = stringSource.replace("\"", "\\\"") # replace '\"' in comments
		stringSource = stringSource.replace("\'", "\\\'") # replace '\'' in comments
	
	stringSource = stringSource.replace("\n", "\\n\"\n\"") # replace '\n'
	stringSource = "const char* ocl_src_" + os.path.splitext(args.inputFileName)[0] + " =\n\"" + stringSource

	if args.compact:
		stringSource += "\";\n"
	else:
		stringSource += "\\n\";\n"

	if args.verbose:
		print("\nOpenCL source as C char string:")
		print(stringSource)

	open(args.outputFileName, "w").write(stringSource)
