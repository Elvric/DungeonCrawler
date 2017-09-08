#!/usr/bin/python

import csv

def main(args):
	# Read room resources from CSV
	with open("resources.csv") as csvfile:
		reader = csv.reader( csvfile, delimiter=',' )
		resourcesCSV = list(reader)
	
	resourcesCSV[0][2] = '0' # Free room
	
	# Write updated resources (empty room) to CSV
	with open('resources.csv','wb') as csvFile:
		writer = csv.writer(csvFile)
		writer.writerows(resourcesCSV)
	
	print('''Content-Type:text/html

<html></html>''')
	
	return 0

if __name__ == '__main__':
	import sys
	sys.exit(main(sys.argv))
