#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// File : room.c
// Room page textbox and player&room resources processing  
// Executable name: a.out.cgi

// Function prototypes:
char* getfield(char* line, int num);
int readCSV(char* input, int manna, int gold);
int readFile(int manna, int gold, int occupied);
void addToFile(int manna, int gold, char occupied);
void endOfGame(int winOrLost , int pManna, int pGold);
void htmlInventory(int manna, int gold, char * message);

int isChoice(char* choice);
int winProcess(char* choice, int myManna, int myGold);

int main(int argc, char * argv[]){
	
	// End of game processing: in function of game.py program response 
	if (argc > 1){
		int winlost = atoi(argv[1]);
		int pManna = atoi(argv[2]);
		int pGold = atoi(argv[3]);
		endOfGame(winlost, pManna, pGold);
	}
	else{ // command processing
		
		char *data = getenv("QUERY_STRING");
		char buffer[100];
		char *token;
		
		// Store the command inputed by the user
		char command[100];
		// Store the player's resources as a CSV string
		char inventory[100];
	
		strncpy(buffer, data, 255);
		token = strtok(buffer, "&");
		// retrieve textbox command in command char array
		sscanf(token,"command=%s",command); 
		// retrieve player's inventory 
		token = strtok(NULL, "&");
		sscanf(token,"inventory=%s",inventory);
		
		
		if(isChoice(command) == 1){ // game winning processing : player and room resources management
			winProcess(command, readCSV(inventory,1,0), readCSV(inventory,0,1));	
		}
		else {
		// Options for commands
		char *play = "PLAY";
		char *drop = "DROP";
		char *exit = "EXIT";
		char *refresh = "REFRESH";
	
		if(strcmp(command,play)==0){ // if command is ---------------------"PLAY"---------------------
			char call[20];
			sprintf(call, "./game.py %d %d", readCSV(inventory,1,0), readCSV(inventory,0,1));
			system(call);
		}
		else if(strncmp(command,drop,4)==0){ // if command is ---------------------"DROP"---------------------
			if(readCSV(inventory,0,1)<= 0){
				int playerManna = readCSV(inventory,1,0);
				int playerGold = readCSV(inventory,0,1);
				printf("Content-Type:text/html\n\n");
				htmlInventory(playerManna, playerGold,"You have no more gold to drop.");
			}
			else{
				int numGoldDrop = 0;
				if (!(command[5] >=48 && command[5] <=57)){ // invalid command so 0 gold are dropped
					numGoldDrop = 0;
				}
				else { // numGoldDrop is between 1 and 10.
					numGoldDrop = (command[6] >= 48)? 10: command[5] - '0';
					//If numGoldDrop > player inventory gold: drop all the gold in player inventory
					if (numGoldDrop > readCSV(inventory,0,1))
						numGoldDrop = 0;		
				}
		
				//Add numGoldDrop to room resources
				int addedGoldToFile = readFile(0,1,0) + numGoldDrop;
				addToFile(-1, addedGoldToFile, 0);
			
				int mannaGained = numGoldDrop/2;
				// Put mannaGained from universe into player inventory
				int playerManna = readCSV(inventory,1,0) + mannaGained;
				// Extract the numGoldDrop from the player's inventory
				int playerGold = readCSV(inventory,0,1) - numGoldDrop;
				
				char dropped[20];
				sprintf(dropped, "You dropped %d golds for %d mannas", numGoldDrop, mannaGained);
		
				printf("Content-Type:text/html\n\n");
				htmlInventory(playerManna, playerGold, dropped);
			}
		}
		else if(strcmp(command,exit)==0){ // if command is ---------------------"EXIT"---------------------
			addToFile(-1, -1, 'n');
			// add player's mannas and golds to room's resources
			int playerManna = readCSV(inventory,1,0);
			int playerGold = readCSV(inventory,0,1);
			int fileManna = readFile(1, 0, 0) + playerManna;
			int fileGold = readFile(0, 1, 0) + playerGold;
			addToFile(fileManna, -1, 0);
			addToFile(-1, fileGold, 0);
			
			// Sorry to leave scene
			printf("Content-Type:text/html\n\n");
			printf("<center><img src=\"https://m.popkey.co/ab0d84/7z76z.gif\" width=\"1200\" height=\"650\"></center>");
			printf("<center><h1> Why would you EXIT!</h1>");
			printf("<h2> At least you gave us your gold ahahah 3:D</h2></center>");
			printf("<body background=\"http://bgfons.com/upload/gold_texture446.jpg\">");
		}
		else if(strcmp(command,refresh)==0){ // if command is ---------------------"REFRESH"---------------------
			// redraws the screen with player's inventory preserved
			int playerManna = readCSV(inventory,1,0);
			int playerGold = readCSV(inventory,0,1);
			printf("Content-Type:text/html\n\n");
			htmlInventory(playerManna, playerGold,"You refreshed!");
		}
		else{ //---------------------INVALID COMMMAND---------------------
			int playerManna = readCSV(inventory,1,0);
			int playerGold = readCSV(inventory,0,1);
			printf("Content-Type:text/html\n\n");
			htmlInventory(playerManna, playerGold,"");
		}
		
	} // for the else after checkWin 
	}
	return 0;
}

// function to tokenize the CSV
char* getfield(char* line, int num)
{
	char* tok;
	for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n")) {
		if (!--num)
			return tok;
	}
	return NULL;
}

// Read player's inventory from CSV string
int readCSV(char* input, int manna, int gold){
	
	char buffer[100];
	char *inventoryCSV;
	strcpy(buffer, input);
	inventoryCSV = buffer;
	
	//char buffer[100];
	char* mannatok = calloc(sizeof(char),2); // Assuming manna < 100
	char* goldtok = calloc(sizeof(char),4); // Assuming gold < 10000
	
	char* mannaptr = mannatok;
	char* goldptr = goldtok;
	
	while (*inventoryCSV != '%') // Get manna left of %2C
	{
		*mannaptr++ = *inventoryCSV;
		*inventoryCSV++ = 0; // "Erase" value in original string
	}
	inventoryCSV+=3; // Skip %2C
	
	while (*inventoryCSV) // Get gold right of %2C
	{
		*goldptr++ = *inventoryCSV;
		*inventoryCSV++ = 0; // "Erase" value in original string
	}
	
	if (manna == 1){
		manna = atoi(mannatok);
		return manna;
	}
	else if (gold == 1){
		gold = atoi(goldtok);
		return gold;
	}
	
	return 0;
}

// Function to read manna, gold or room status from resources.csv file
int readFile(int manna, int gold, int occupied){
	//Initialize the file to be opened and open it
	FILE * file;
	file = fopen("resources.csv", "r+");
	char tmp[10];
	char *resources;
	char ch;
	int i = 0;
	
	ch = fgetc(file); // get character from file
	while (ch != EOF)
	{
		tmp[i] = ch;
		i++;
		ch = fgetc(file);
	}
	resources = tmp;
	
	if (manna == 1){
		manna = atoi(getfield(resources, 1));
		return manna;
	}
	else if (gold == 1){
		gold = atoi(getfield(resources, 2));
		return gold;	
	}
	else if (occupied == 1){
		occupied = atoi(getfield(resources, 3));
		return occupied;
	}
	return 0;
}

// Function to add (write) manna, gold or room status to resources.csv file
void addToFile(int manna, int gold, char occupied){
	
	int mannaInFile = readFile(1, 0, 0);
	int goldInFile = readFile(0, 1, 0);
	int occupiedInFile = readFile(0, 0, 1);
	long int counter = 0;
	FILE * file;
	file = fopen("resources.csv", "w");
	if (manna >= 0)
		fprintf(file, "%d%s%d%s%d", manna, ",", goldInFile, ",", occupiedInFile);
	else if (gold >= 0)
		fprintf(file, "%d%s%d%s%d", mannaInFile, ",", gold,",", occupiedInFile);
	else if (occupied == 'y'){ // occupied room
		fprintf(file, "%d%s%d%s%d", mannaInFile,",", goldInFile,",", 1);
	}
	else if (occupied == 'n'){ // not occuppied room
		fprintf(file, "%d%s%d%s%d", mannaInFile,",", goldInFile,",", 0);
	}
	fclose(file);
}

// -------------- End of game function: the following will happen when the game terminates. --------------
void endOfGame(int winOrLost , int pManna, int pGold){
	if (winOrLost == 1){ // WIN
		printf("Content-Type:text/html\n\n");
		// player choose 5 items
		printf("<body background=\"https://scontent.fyhu1-1.fna.fbcdn.net/v/t1.0-9/17522953_150734175453645_3221980829725637494_n.jpg?oh=3dc4c6bdaca56ed8307777fd1a079db1&oe=598BA9E6\">");
		printf("<center><h1>CONGRATS. YOU WON! Choose 5 items from our room.</h1></center>");
		printf("<center><h1>The room currently has %d mannas and %d golds.</h1></center>", readFile(1,0,0),readFile(0,1,0));
			
		printf("<form action = \"/~jzhang207/cgi-bin/a.out.cgi\">"); 
		printf("<center><input type=\"radio\" name=\"command\" value=\"FZ\" checked> <b> 5 mannas and 0 golds </b> </center><br>");
		printf("<center><input type=\"radio\" name=\"command\" value=\"FO\"> <b> 4 mannas and 1 golds </b></center><br>");
		printf("<center><input type=\"radio\" name=\"command\" value=\"THT\"> <b> 3 mannas and 2 golds </b></center><br>"); 
		printf("<center><input type=\"radio\" name=\"command\" value=\"TTH\"> <b> 2 mannas and 3 golds </b></center><br>");
		printf("<center><input type=\"radio\" name=\"command\" value=\"OF\"> <b> 1 mannas and 4 golds </b></center><br>");
		printf("<center><input type=\"radio\" name=\"command\" value=\"ZF\"> <b> 0 mannas and 5 golds </b></center><br><br>");
		printf("<center><input type=\"hidden\" name=\"inventory\" value=\"%d,%d\"></center><br><br>", pManna, pGold);
		printf("<center><input type=\"submit\" value=\"FEED ME\"name=\"reward\"></form>"); 
		printf("</body>");
	}
	else if (winOrLost == 0){ // LOST
		// End of game page
		printf("Content-Type:text/html\n\n");
		// Checking if the player can still lose golds
		int tmp;
		if(pGold>=3){
			pGold=pGold-3;
			tmp = readFile(0, 1, 0) + 3;
		}
		else{
			tmp = readFile(0, 1, 0) + pGold;
			pGold=0;
		}
		htmlInventory(pManna, pGold, "YOU JUST LOST 3 GOLDS !");
		addToFile(-1, tmp, 0);
	}
}

// print the room.html page
void htmlInventory(int manna, int gold, char * message){
   printf("<!DOCTYPE html>");
   printf("<!-- ROOM.HTML: Main room html page -->");
   printf("<html>");
   printf("<title>COMP206 A4</title>");
   printf("<body background=\"https://scontent.fyhu1-1.fna.fbcdn.net/v/t1.0-9/17522967_150746458785750_1310174469530381337_n.jpg?oh=d68905427645b53c1ea03facdbc67b85&oe=598B7D9A\">");
   printf("	<center><img src=\"http://hbu.h-cdn.co/assets/15/15/1428509417-target-gif-sun-room.gif\" alt=\"Room\" width=\"800\" height=\"425\"></center>");
   printf("	<center><h2> Welcome! %s </h2></center>", message);
   printf("	<center><font color=\"#001a33\"><h3> YOU CAN: PLAY| DROP n | EXIT | REFRESH </h3></font></center>");
   printf("	<!-- Teleportation button processing using p.py -->"); 
	// -------------------------------- Teleportation section --------------------------------
	// TEAM 1 : NORTH
	printf("<form name=\"input\" action=\"/~rke/cgi-bin/transporter.py\" method=\"get\">"); // URL of other team
	printf("	<input type=\"hidden\" name=\"inventory\" value=\"%d,%d\">", manna, gold);
	printf("	<input type=\"hidden\" name=\"url\" value=\"https://www.cs.mcgill.ca/~jzhang207\">");
	printf("	<center><input type=\"submit\" value=\"NORTH\" name=\"north\"></center> ");
	printf("</form>");
	// TEAM 2 : WEST
	printf("<div style=\"float: left; padding-left: 500px\">");
	printf("<form name=\"input\" action=\"/~syu47/cgi-bin/transporter.py\" method=\"post\">"); // URL of other team
	printf("	<input type=\"hidden\" name=\"inventory\" value=\"%d,%d\">", manna, gold);
	printf("	<input type=\"hidden\" name=\"URL\" value=\"https://www.cs.mcgill.ca/~jzhang207/cgi-bin/a.out.cgi\">");
	printf("	<center><input type=\"submit\" value=\"WEST\" name=\"west\"></center>");
	printf("</form>");
	printf("</div>");
	// TEAM 3 : EAST
	printf("<div style=\"float: right; padding-right: 500px\">");
	printf("<form name=\"input\" action=\"/~hnguye124/cgi-bin/transporter.py\" method=\"get\">"); // URL of other team
	printf("	<input type=\"hidden\" name=\"inventory\" value=\"%d,%d\">", manna, gold);
	printf("	<input type=\"hidden\" name=\"URL\" value=\"https://www.cs.mcgill.ca/~jzhang207/cgi-bin/a.out.cgi?command=REFRESH&inventory=%d%%2C%d&submit=SUBMIT\">", manna, gold);
	printf("	<center><input type=\"submit\" value=\"EAST\" name=\"east\"></center>");
	printf("</form>");
	printf("</div><br>");
	// TEAM 4 : SOUTH
	printf("<form name=\"input\" action=\"/~ppoppe/cgi-bin/transporter.py\" method=\"post\">"); // URL of other team
	printf("	<input type=\"hidden\" name=\"inventory\" value=\"%d,%d\">", manna, gold);
	printf("	<input type=\"hidden\" name=\"URL\" value=\"https://www.cs.mcgill.ca/~jzhang207/cgi-bin/a.out.cgi?command=REFRESH&inventory=%d%%2C%d&submit=SUBMIT\">", manna, gold);
	printf("	<center><input type=\"submit\" value=\"SOUTH\" name=\"south\"></center>");
	printf("</form>");
	printf("<br>");
	// Textbox and command process section:
   printf("	<!-- Textbox processing using a.out.cgi executable -->"); 
   printf("	<form name=\"input\" action=\"/~jzhang207/cgi-bin/a.out.cgi\" method=\"get\">");
   printf("		<!-- Command textbox -->");
   printf("		<center><input type = \"text\" id = \"myText\" name = \"command\"></center>");
   printf("		<!-- Player's inventory -->");
	printf("		<input type=\"hidden\" name=\"inventory\" value=\"%d,%d\">", manna, gold);
   printf("		<!-- Submit button -->");
   printf("		<center><input type=\"submit\" value=\"SUBMIT\" name = \"submit\"></center>");
   printf("	</form>");
   printf("</body>");
}

// ----------------------------GAME WIN PROCESSING: PLAYER & ROOM RESOURCES MANIPULATION--------------------------

int isChoice(char* choice){
	if (strcmp(choice,"FZ")==0 || strcmp(choice,"FO")==0 || strcmp(choice,"THT")==0 || strcmp(choice,"TTH")==0|| strcmp(choice,"OF")==0 || strcmp(choice,"ZF")==0) 
		return 1;
	else
		return 0;
}

// Function that processes the reward for the player after winning the game and taking away resources from our room
int winProcess(char* choice, int myManna, int myGold){
	if(strcmp(choice,"FZ")==0){
		// Checking if room is out of resources		
		int preManna = readFile(1, 0, 0);
		if (preManna - 5 < 0){
			printf("Content-Type:text/html\n\n");
			htmlInventory(myManna, myGold,"<font color=\"#ff0000\">Sorry.</font> You gained nothing: our room is out of resources!");
			return 0;
		}
	
		// remove from resources.csv file (room resources file): 5 mannas, 0 golds
		int mannas = readFile(1, 0, 0) - 5;
		addToFile(mannas, -1, 0);
		
		printf("Content-Type:text/html\n\n");
		htmlInventory(myManna + 5, myGold,"You gained 5 mannas. Excellent!");
		return 0;
	}
	else if(strcmp(choice,"FO")==0){ 
		// Checking if room is out of resources
		int preManna = readFile(1, 0, 0);
		int preGold = readFile(0, 1, 0);
		if (preManna - 4 < 0 || preGold - 1 < 0){
			printf("Content-Type:text/html\n\n");
			htmlInventory(myManna, myGold,"<font color=\"#ff0000\">Sorry.</font> You gained nothing: our room is out of resources!");
			return 0;
		}
		
		// remove from resources.csv file (room resources file): 4 mannas, 1 golds
		int mannas = readFile(1, 0, 0) - 4;
		int golds = readFile(0, 1, 0) - 1;
		addToFile(mannas, -1, 0);
		addToFile(-1, golds, 0);
		myGold = myGold + 1;
		if(myGold>=100){
			addToFile(-1, -1, 'n');
			printf("Content-Type:text/html\n\n");
			printf("<center><img src=\"https://scontent.fyhu1-1.fna.fbcdn.net/v/t1.0-0/p206x206/17795825_147709902422739_5554476431206356789_n.jpg?oh=eada8e9b88ed0ff3b984811a9177031b&oe=5964D3C3\" alt=\"Room\" width=\"1200\" height=\"650\"></center>");
			return 0;
		}
		printf("Content-Type:text/html\n\n");
		htmlInventory(myManna + 4, myGold,"You gained 4 mannas and 1 gold. Bravo!");
		return 0;
	}
	else if(strcmp(choice,"THT")==0){ 
		// Checking if room is out of resources
		int preManna = readFile(1, 0, 0);
		int preGold = readFile(0, 1, 0);
		if (preManna - 3 < 0 || preGold - 2 < 0){
			printf("Content-Type:text/html\n\n");
			htmlInventory(myManna, myGold,"<font color=\"#ff0000\">Sorry.</font> You gained nothing: our room is out of resources!");
			return 0;
		}
		
		// remove from resources.csv file (room resources file): 3 mannas, 2 golds
		int mannas = readFile(1, 0, 0) - 3;
		int golds = readFile(0, 1, 0) - 2;
		addToFile(mannas, -1, 0);
		addToFile(-1, golds, 0);
		myGold = myGold+2;
		if(myGold>=100){
			addToFile(-1, -1, 'n');
			printf("Content-Type:text/html\n\n");
			printf("<center><img src=\"https://scontent.fyhu1-1.fna.fbcdn.net/v/t1.0-0/p206x206/17795825_147709902422739_5554476431206356789_n.jpg?oh=eada8e9b88ed0ff3b984811a9177031b&oe=5964D3C3\" alt=\"Room\" width=\"1200\" height=\"650\"></center>");
			return 0;
		}
		printf("Content-Type:text/html\n\n");
		htmlInventory(myManna + 3, myGold,"You gained 3 mannas and 2 golds. Good Job!");
		return 0;
	}
	else if(strcmp(choice,"TTH")==0){ 
		// Checking if room is out of resources
		int preManna = readFile(1, 0, 0);
		int preGold = readFile(0, 1, 0);
		if (preManna - 2 < 0 || preGold - 3 < 0){
			printf("Content-Type:text/html\n\n");
			htmlInventory(myManna, myGold,"<font color=\"#ff0000\">Sorry.</font> You gained nothing: our room is out of resources!");
			return 0;
		}
		
		// remove from resources.csv file (room resources file): 2 mannas, 3 golds
		int mannas = readFile(1, 0, 0) - 2;
		int golds = readFile(0, 1, 0) - 3;
		addToFile(mannas, -1, 0);
		addToFile(-1, golds, 0);
		myGold = myGold + 3;
		if(myGold>=100){
			addToFile(-1, -1, 'n');
			printf("Content-Type:text/html\n\n");
			printf("<center><img src=\"https://scontent.fyhu1-1.fna.fbcdn.net/v/t1.0-0/p206x206/17795825_147709902422739_5554476431206356789_n.jpg?oh=eada8e9b88ed0ff3b984811a9177031b&oe=5964D3C3\" alt=\"Room\" width=\"1200\" height=\"650\"></center>");
			return 0;
		}
		printf("Content-Type:text/html\n\n");
		htmlInventory(myManna + 2, myGold,"You gained 2 mannas and 3 golds. Nice work!");
		return 0;
	}
	else if(strcmp(choice,"OF")==0){
		// Checking if room is out of resources
		int preManna = readFile(1, 0, 0);
		int preGold = readFile(0, 1, 0);
		if (preManna - 1 < 0 || preGold - 4 < 0){
			printf("Content-Type:text/html\n\n");
			htmlInventory(myManna, myGold,"<font color=\"#ff0000\">Sorry.</font> You gained nothing: our room is out of resources!");
			return 0;
		}
		
		// remove from resources.csv file (room resources file): 1 mannas, 4 golds
		int mannas = readFile(1, 0, 0) - 1;
		int golds = readFile(0, 1, 0) - 4;
		addToFile(mannas, -1, 0);
		addToFile(-1, golds, 0);
		myGold = myGold + 4;
		if(myGold>=100){
			addToFile(-1, -1, 'n');
			printf("Content-Type:text/html\n\n");
			printf("<center><img src=\"https://scontent.fyhu1-1.fna.fbcdn.net/v/t1.0-0/p206x206/17795825_147709902422739_5554476431206356789_n.jpg?oh=eada8e9b88ed0ff3b984811a9177031b&oe=5964D3C3\" alt=\"Room\" width=\"1200\" height=\"650\"></center>");
			return 0;
		}
		printf("Content-Type:text/html\n\n");
		htmlInventory(myManna + 1, myGold,"You gained 1 manna and 4 golds. Amazing!");
		return 0;
	}
	else if(strcmp(choice,"ZF")==0){
		// Checking if room is out of resources
		int preGold = readFile(0, 1, 0);
		if (preGold - 5 < 0){
			printf("Content-Type:text/html\n\n");
			htmlInventory(myManna, myGold,"<font color=\"#ff0000\">Sorry.</font> You gained nothing: our room is out of resources!");
			return 0;
		}	
		
		// remove from resources.csv file (room resources file): 0 mannas, 5 golds
		int golds = readFile(0, 1, 0) - 5;
		addToFile(-1, golds, 0);
		myGold = myGold + 5;
		if(myGold>=100){
			addToFile(-1, -1, 'n');
			printf("Content-Type:text/html\n\n");
			printf("<center><img src=\"https://scontent.fyhu1-1.fna.fbcdn.net/v/t1.0-0/p206x206/17795825_147709902422739_5554476431206356789_n.jpg?oh=eada8e9b88ed0ff3b984811a9177031b&oe=5964D3C3\" alt=\"Room\" width=\"1200\" height=\"650\"></center>");
			return 0;
		}
		printf("Content-Type:text/html\n\n");
		htmlInventory(myManna, myGold,"You gained 5 golds. Fabulous!");
		return 0;
	}
	return 0;
}

