#!/usr/bin/python
#Coded by Elvric Trombert
import random
import sys
import csv
import cgi
import subprocess
import shlex
""" Notes on the csv file:
        first row encodes the location of the word printed previously as well as the number of trials left
        second,thrid and forth encodes the word subject first is comp sci second is math and so on
        the other rows encode the hints. We set the access of the wordList.csv private so that the player will not be able to cheet and read the words."""

        #purpose: select word randomly from the subject section wanted by the user
def sendWord(num):
                #open file generate a random number that select a word in the list
        f=open('wordList.csv','rb')
        readCsv=csv.reader(f,delimiter=',')
        words=list(readCsv)
        numS=random.randint(0,len(words[num])-1)
                #store the word coodinates about to be displayed in the file
        words[0][0]=num
        words[0][1]=numS
        word=[words[num][numS],words[num+3][numS]]
                #copy the new information back into the file
        f=open('wordList.csv','wb')
        writer=csv.writer(f)
        writer.writerows(words)
        f.close()
                #return a list containing the hint and the word
        return word

def shuffle(word):
        wordA=list(word)
        i=0
                #loop that goes through every letter of the word and swap them randomly this is done 10 times to ensure a good shuffle
        for j in range(0,9):
			for i in range(0,len(wordA)-1):
					switch=random.randint(0,len(wordA)-1)
					wordA[i], wordA[switch]=wordA[switch], wordA[i]
					i+=1
        wordA=''.join(wordA)
                #return the shuffled word
        return wordA

                #purpose: retreive a scramble word
def playGame(num):
        word=sendWord(num)
        wordList=[shuffle(word[0]),word[1]]
        return wordList

                #check if the input entered by the user is correct and matches the scrambled word
def testWin(wordUser):
        f=open('wordList.csv','r+')
        readCsv=csv.reader(f,delimiter=',')
        wordL=list(readCsv)
        num=wordL[0]
        wordTest=wordL[int(num[0])][int(num[1])]
        answer=wordUser==wordTest
        return answer
                #print the hml menu where the user get to choose the topic in which the word will be
def gameChoose(message):
        print "Content-type: text/html\n\n"
        print ('''
				<html>
				<head>
				<style>
				div.background {
				  background: url(http://theromantic.com/wp-content/uploads/2015/07/romantic-games-scrabbling-.jpg);
				 position: fixed;
				width: 100%%;
				height: 100%%;
				left: 0;
				top: 0;
				}
				div.transbox{
				  background-color: #ffffff;
				  border: 1px solid black;
				  opacity: 0.85;
				  filter: alpha(opacity=60); 
				position: absolute;
				width: 50%%;
				height: 75%%;
				transform: translateX(50%%) translateY(50%%);
				}
				</style>
				</head>
				<body>	
				<center>
				<div class="background">
				  <div class="transbox">
				  <h1>Find the scramble word</h1>
				  %s
				  </center>
				  </div>
				</div>
				</body>
				</html>
                '''%(message))
                #count the number of time the user has tried the game and adds losses if he has not tried more than 2 take a string variable as input to reset loos count to 0 if payer wins
def lost(status):
        answer=True
        f=open('wordList.csv','rb')
        readCsv=csv.reader(f,delimiter=',')
        words=list(readCsv)
        numLoss=words[0][2]
        if numLoss=="1" or status=="n":
                words[0][2]=3
                answer=False
        else:
                words[0][2]=int(numLoss)-1

        f=open('wordList.csv','wb')
        writer=csv.writer(f)
        writer.writerows(words)
        f.close()
        return answer
        
def numCount():
        f=open('wordList.csv','rb')
        readCsv=csv.reader(f,delimiter=',')
        words=list(readCsv)
        numLoss=words[0][2]
        f.close()
        return numLoss

def main(argv):
				#creates a reader that reads the variable passed by the html file
		form=cgi.FieldStorage()
				#check if this is the first time the game is ran and prints the radio list with first time message
		if len(argv)==3:
				#reset the number of trials back to 3 incase the previous user did not exit properly.
			lost("n")
			inventory=[argv[1],argv[2]] 
			stringToPrint='''<center><h2>Choose the subject/categorie of the word, press GO! to start, you have %s trials left</h2> 
					<h3>(if you quit or use up all your trials, you will give us one gold mouahahah)</h3>
					</center><center><p></p><h6>You can type QUIT at anytime to leave the game</h6></center>
					<form name=\"input\" action=\"./game.py\" method=\"get\">
					<input type=\"hidden\" name=\"inventory\" value=\"%s,%s\">
					<center><input type=\"radio\" name=\"topic\" value=\"1\" checked> Computer Science
							<input type=\"radio\" name=\"topic\" value=\"2\"> Math
							<input type=\"radio\" name=\"topic\" value=\"3\"> Physics<br>
							<center><input type = \"text\" id = \"answer\" name = \"answer\"></center>
							<input type=\"submit\" value=\"GO!\" name = \"submit\">
					</form>'''%(numCount(),inventory[0],inventory[1],)
			gameChoose(stringToPrint)
			return 0
			#if the first if does not run then the player inventory exists
		inventory=form.getvalue("inventory")
		inventory=inventory.split(',')
			#check if the player chose a topic or entered a word
		if form.has_key("topic"):
						#read what topic the user chose and call the select word to be printed on the game room
			topic=form.getlist("topic")
			wordUser=form.getvalue("answer")
			if wordUser=="QUIT":
				lost("n")
				subprocess.call(["./a.out.cgi", "0", inventory[0], inventory[1]])
				return 0
				#based on the topic we select the row that we must go to on the word file
			value=int(topic[0])
			word=playGame(value)
			stringToPrint='''<center>
						<h2>%s</h2>
						<h3> Hint: %s</h3>
						<center><p></p><h5>rearrange the letter displayed to form a word.</h5></center>
				<center><p></p><h6>You can type QUIT at anytime to leave the game</h6></center>
				<form name=\"input\" action=\"./game.py\" method=\"get\">
				<input type=\"hidden\" name=\"inventory\" value=\"%s,%s\">
				<center><input type = \"text\" id = \"answer\" name = \"answer\"></center>
				<center><input type=\"submit\" value=\"Validate guess\" name = \"submit\"></center>
				</form>'''%(word[0],word[1],inventory[0],inventory[1],)
			gameChoose(stringToPrint)
				#the user has guessed a word
		else:
					#take that string test if it is QUIT if so quit the game other wise check if the user guess is valid
				wordUser=form.getvalue("answer")
				if wordUser=="QUIT":
						lost("n")
						subprocess.call(["./a.out.cgi", "0", inventory[0], inventory[1]])
						return 0
				guess=testWin(wordUser)
					#if it is valid call c program with 1 as that is the feedback to send if player wins, 0 otherwise
				if guess:
						lost("n")
						subprocess.call(["./a.out.cgi", "1", inventory[0], inventory[1]])
				else:
							#the player guess is wrong we check if the player still has some trials left if so we print back the topic choice page
						answer=lost("y")
						if answer:
									#if player word guess is wrong print the radio page again with knew message
								stringToPrint='''<center><h2>Incorrect guess try again, you have %s trials remaining</h2> 
					<h3>(if you quit or use up all your trials, you will give us 3 gold mouahahah)</h3>
					</center><center><p></p><h6>You can type QUIT at anytime to leave the game</h6></center>
					<form name=\"input\" action=\"./game.py\" method=\"get\">
					<input type=\"hidden\" name=\"inventory\" value=\"%s,%s\">
					<center><input type=\"radio\" name=\"topic\" value=\"1\" checked> Computer Science
							<input type=\"radio\" name=\"topic\" value=\"2\"> Math
							<input type=\"radio\" name=\"topic\" value=\"3\"> Physics<br>
							<center><input type = \"text\" id = \"answer\" name = \"answer\"></center>
							<input type=\"submit\" value=\"GO!\" name = \"submit\">
					</form>'''%(numCount(),inventory[0],inventory[1],)
								gameChoose(stringToPrint)
							#the player has no trials left reset the num of trials back to 3 and call the c program.
						else:
							lost("n")
							subprocess.call(["./a.out.cgi", "0", inventory[0], inventory[1]])
if __name__ == "__main__": main(sys.argv)
