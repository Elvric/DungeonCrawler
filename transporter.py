#!/usr/bin/python

# transporter.py

# Import modules for CGI handling, process calling
import cgi, cgitb, csv, urllib, urllib2, requests
#~ cgitb.enable()
#~ from subprocess import call

############################# USEFUL LINKS ##############################
# https://docs.python.org/2/library/cgi.html							         #
# https://en.wikipedia.org/wiki/Common_Gateway_Interface				      #
# https://docs.python.org/2/library/csv.html							         #
# https://www.tutorialspoint.com/python/python_cgi_programming.htm		#
#########################################################################

def getURL():
	# Get the URL
	url = os.environ.get("SERVER_NAME", '') + os.environ.get("REQUEST_URI", '')
	return url

# Check our room's occupancy
def checkOccupied(resourcesCSV, prevURL, inventory):
	if ( int(resourcesCSV[0][2]) ): # Occupied
		#########################################################
		# Use URL to call C program to regenerate previous room #
		#########################################################
		print('''<body style="background-image:url(https://wallpapers.wallhaven.cc/wallpapers/full/wallhaven-271486.jpg)">
<center><h1>Welcome! Please wait while we redirect you to the room...</h1></center>
<center><h2>WARNING! The room is currently occupied! Please wait while we redirect you back to try again...</h2></center>
<center><img src="http://i7.photobucket.com/albums/y300/evillordzog/red-alert.gif" alt="WARNING: ROOM OCCUPIED" width="150" height="150"></center>
<p></p>
<center> &nbsp &nbsp &nbsp &nbsp <img src="http://vignette4.wikia.nocookie.net/custombionicle/images/5/53/Loading_bar.gif/revision/20150122161333" alt="Loading..." width="940" height="120"></center>
''')
		if ('hnguye124' in prevURL):
			print('<meta http-equiv="refresh" content="3;url=%s" />' % (prevURL))
		elif ('~rke' in prevURL):
			print('''<center>
<form action="%s" method="POST">
<input type="hidden" name="inventory" value="%s,%s">
<input type="hidden" name="command" value="REFRESH">
<input type="submit" value="Oh. Welp. Time to go back!">
</form>
</center>''' % (prevURL,inventory[0],inventory[1]))
		elif ('ppoppe' in prevURL):
			print('''<center>
<form action="%scgi-bin/room.cgi" method="GET">
<input type="hidden" name="command" value="REFRESH">
<input type="hidden" name="inventory" value="%s,%s">
<input type="submit" value="The room is occupied, better go back!">
</form>
</center>''' % (prevURL,inventory[0],inventory[1]))
		elif ('syu47' in prevURL):
			print('''<center>
<form action="%s" method="POST">
<input type="hidden" name="inventory" value="%s,%s">
<input type="hidden" name="command" value="REFRESH">
<input type="submit" value="Oh. Welp. Time to go back!">
</form>
</center>''' % (prevURL,inventory[0],inventory[1]))
		elif ('jnimij' in prevURL): 
			print('''<center>
<form action="%s" method="GET">
<input type="hidden" name="inventory" value="%s,%s">
<input type="hidden" name="command" value="REFRESH">
<input type="submit" value="Back to the prvious room I go!">
</form>
</center>''' % (prevURL,inventory[0],inventory[1]))
		else: # Generic case
			print('''<center>
<form action="%s" method="POST">
<input type="hidden" name="command" value="REFRESH">
<input type="hidden" name="inventory" value="%s,%s">
<input type="submit" value="<---">
</form>
</center>''' % (prevURL,inventory[0],inventory[1]))
		
		return 1
	else: # Empty
		######################################################
		# Use URL to tell previous room successful transport #
		######################################################
		inventory[0] = int(inventory[0]) - 1 # Deduct a manna
		resourcesCSV[0][0] = int(resourcesCSV[0][0]) + 1 # Add it to the room
		if ( int(inventory[0]) < 1 ): # If manna 0, death screen
			print('''<body style="background-image:url(https://i.ytimg.com/vi/zTHhwvTJyMU/hqdefault.jpg)">
<center><img src="http://i7.photobucket.com/albums/y300/evillordzog/red-alert.gif" alt="Red alert" width="150" height="150"></center>
<center><img src="http://scossavr.altervista.org/Common/Icone/game_over_02.gif" alt="Game over" width="400" height="200"></center>
<center><h2> /!\ No more manna :( /!\ </h2></center>
<center><h2>You have died of dysentry.</h2></center>
<center><img src="http://bestanimations.com/Humans/Skulls/Skull-43-june.gif" alt="DEATH" width="200" height="200"></center>
<form action="https://www.youtube.com/watch">
<input type="hidden" name="v" value="dQw4w9WgXcQ">
<center><input type="image" src="death.png" alt="Totally legit death ad" width="700" height="300"></center>
</form>
</body>
</html>
''')
			return -1
		
		resourcesCSV[0][2] = '1' # Make room occupied
		csvFile = open('resources.csv','wb')
		writer = csv.writer(csvFile)
		writer.writerows(resourcesCSV)
		csvFile.close()
		
		genRoom(inventory)
		
		# Send success signal
		successURL = prevURL
		if ('~rke' in prevURL):
			print('''<form action="http://cs.mcgill.ca/~rke/cgi-bin/room.cgi" method="GET">
<input type = "hidden" name = "inventory" value = "%s,%s">
<input type = "hidden" name ="cmd" value="REFRESH">
<label><input type="submit" value="Go to the room!"></label>
</form>
<object type= "text/html" width="0" height="0" data="http://cs.mcgill.ca/~rke/cgi-bin/success.py">
</object>
</div>
</br>''' % (inventory[0],inventory[1]))
		elif ('jnimij' in prevURL):
			print('''<form action="http://cs.mcgill.ca/~jnimij/cgi-bin/success.py" method="GET">
<input type="hidden" name="url" value="http://cs.mcgill.ca/~jzhang207/cgi-bin/a.out.cgi?command=REFRESH&inventory=%s%%2C%s">
<label><input type="submit" value="Success! Git to tha room-ah!"></label>
</form>''' % (inventory[0],inventory[1]))
		elif ('syu47' in prevURL): successURL = 'https://www.cs.mcgill.ca/~syu47/cgi-bin/success.py'
		
		requests.posts(url=successURL, data=None)
		
		return 0

# Generate our room with inventory using a.out (room.c)
def genRoom(inventory):
	print('''<body style="background-image:url(https://wallpapers.wallhaven.cc/wallpapers/full/wallhaven-271486.jpg)">
<center><h1>Welcome! Please wait while we redirect you to the room...</h1></center>
<center><h2>MAIN SYSTEMS: ONLINE. ROOM INCOMING AND IMMINENT. PREPARE FOR BATTLE!</h2></center>
<center><img src="http://i7.photobucket.com/albums/y300/evillordzog/red-alert.gif" alt="SUCCESS" width="150" height="150"></center>
<center> &nbsp &nbsp &nbsp &nbsp <img src="http://vignette4.wikia.nocookie.net/custombionicle/images/5/53/Loading_bar.gif/revision/20150122161333" alt="Loading..." width="940" height="120"></center>
''')
	roomURL = ("https://www.cs.mcgill.ca/~jzhang207/cgi-bin/a.out.cgi?input=REFRESH&inventory=%s%s%s&submit=SUBMIT" % (inventory[0],'%2C',inventory[1]))
	print('<meta http-equiv="refresh" content="3;url=%s" />' % (roomURL))
	resetGame()

# Reset minigame as extra caution
def resetGame():
	f=open('wordList.csv','rb')
	readCsv=csv.reader(f,delimiter=',')
	words=list(readCsv)
	words[0][2]=3
	f=open('wordList.csv','wb')
	writer=csv.writer(f)
	writer.writerows(words)
	f.close()

def main(args):
	print("Content-Type:text/html\n\n")
	print('''<!DOCTYPE html>
<html>
<title>COMP206 A4</title>
<head>
<style>
@-webkit-keyframes blink {
	0% {
		opacity: 0;
	}
	50% {
		opacity: 1;
	}
	100% {
		opacity: 0;
	}
}
@-moz-keyframes blink {
	0% {
		opacity: 0;
	}
	50% {
		opacity: 1;
	}
	100% {
		opacity: 0;
	}
}
@-o-keyframes blink {
	0% {
		opacity: 0;
	}
	50% {
		opacity: 1;
	}
	100% {
		opacity: 0;
	}
}
img {
	opacity: 0.5;
	filter: alpha(opacity=50);
	-webkit-animation: blink 1.2s;
	-webkit-animation-iteration-count: infinite;
	-moz-animation: blink 1.2s;
	-moz-animation-iteration-count: infinite;
	-o-animation: blink 1.2s;
	-o-animation-iteration-count: infinite;
}
body {
	background-size: cover;
	background-repeat: no-repeat;
	padding-top: 40px;
}
h1 {
		color: #1CA51E;
}
h2 {
		color: #D24015;
}
</style>
</head>''')
	
	# Create instance of FieldStorage and check and get inventory value and previous room URL
	form = cgi.FieldStorage()
	
	inventoryField = form.getvalue("inventory", "Missing inventory") # Get inventory
	inventory = inventoryField.split(',') # Get array with manna first and gold second
	
	prevURL = form.getvalue("URL", "Missing previous room URL") # Get URL
	if ("Missing previous room URL" in prevURL): prevURL = form.getvalue("url", "Missing previous room URL")
	
	if ( ("Missing " in inventoryField) and ("Missing " in prevURL) ):
		inventory[0] = '2' # Starting manna (gets 1 deducted later)
		inventory += ['0'] # Starting gold
		prevURL = 'https://cgi.cs.mcgill.ca/~jzhang207/cgi-bin/transporter.py' # Initial previous URL
		################################################################################
		# Previous URL to keep refreshing and attempting to connect until room is free #
		################################################################################
	elif ("Missing inventory" in inventoryField): # If missing inventory field in URL
		print('''<body style="background-image:url(https://wallpapers.wallhaven.cc/wallpapers/full/wallhaven-271486.jpg)">
<center><h1>Welcome! Please wait while we redirect you to the room...</h1></center>''')
		print("<center><h2>Oops! %s</h2></center>" % (inventoryField))
		print('<center><img src="http://i7.photobucket.com/albums/y300/evillordzog/red-alert.gif" alt="WARNING: NO INVENTORY" width="150" height="150"></center>')
		print("<p></p>")
		print('<center> &nbsp &nbsp &nbsp &nbsp <img src="http://vignette4.wikia.nocookie.net/custombionicle/images/5/53/Loading_bar.gif/revision/20150122161333" alt="Loading..." width="940" height="120"></center>')
		print('''</body>
</html>
''')
		return 1
	elif ( prevURL is "Missing previous room URL" ): # If missing URL
		print('''<body style="background-image:url(https://wallpapers.wallhaven.cc/wallpapers/full/wallhaven-271486.jpg)">
<center><h1>Welcome! Please wait while we redirect you to the room...</h1></center>
<center><h2>Oops! %s</h2></center>")
<center><img src="http://i7.photobucket.com/albums/y300/evillordzog/red-alert.gif" alt="WARNING: NO URL" width="150" height="150"></center>
<center> &nbsp &nbsp &nbsp &nbsp <img src="http://vignette4.wikia.nocookie.net/custombionicle/images/5/53/Loading_bar.gif/revision/20150122161333" alt="Loading..." width="940" height="120"></center>
</body>
</html>
''' % (prevURL))
		return 1
	
	# Read room resources from CSV
	with open("resources.csv") as csvfile:
		reader = csv.reader( csvfile, delimiter=',' )
		resourcesCSV = list(reader)
	
	# Check occupancy of this room and generate this room if empty
	checkOccupied( resourcesCSV, prevURL, inventory )
	
	print('''</body>
</html>
''')
	
	return 0

if __name__ == '__main__':
	import sys
	sys.exit(main(sys.argv))
