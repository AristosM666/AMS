#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "algrtms.h"

typedef enum { FALSE, TRUE } boolean;

const char ARCHIVE[] = "ams_archive.txt";
const char TMP_FILE[] = "tmp.txt";

#define ID_COL '1'
#define COLOR_COL '2'
#define MANUFACT_COL '3'
#define DATE_COL '4'
const short MIN_DATE = 1960;
const short MAX_DATE = 2006;
const short MIN_ID = 1000;
const short MAX_ID = 9999;
typedef struct {
	char id[5];
	char color[30];
	char manufact[30];
	char date[5];
} Car;

typedef struct {
	size_t lineNum;
	enum { INVALID,	REMOVE,	WRITE } opType;
} Operation;

/* Returns a string of all UNIQUE numeric values in the DATE or ID fields */
char* findAllNumeric(FILE* fp, char col);
/* Returns all UNIQUE alpha-numeric strings in the COLOR or MANUFACT fields */
char* findAllAlphaNumeric(FILE* fp, char col);
/* Returns the line number of the first matching entry */
size_t searchArchive(FILE* fp, char* key, char col, char sign);
/* Overwrites/Removes archive entry */
boolean writeToArchive(FILE* fp, size_t lineNum, Car* car);
void displayEntry(FILE* fp, size_t lineNum); 
void displayEntrys(FILE* fp);
void enterCarInfo(Car* car);
/* Initialize a Car with empty strings and return it */
Car createNullCar(void);
/* display error message and exit */
void fatal(char* errMsg);
void clearScreen(void);
/* Get line count from file */
size_t getLineCount(FILE* fp);
void printMainMenu(void);
void printSearchMenu(void);

int main(void)
{
	char option, searchKey[30], *tmpPtr;
	boolean cont = TRUE;
	FILE* archiveFp;
	Operation operation;
	Car car;
	
	system("TITLE Automotive Managment System");

	archiveFp = fopen(ARCHIVE, "r+");
	if (archiveFp == NULL)
		fatal("Opening Archive!");

	do {
		printMainMenu();
		scanf("%c", &option);

		clearScreen();
		rewind(archiveFp);
		getchar();

		switch (option)
		{
		case '1':
			printf("\n\n\t\t[*] View Archive Entrys [*]\n\n");
			displayEntrys(archiveFp);

			printf("\n\n\t\tPress Any Key To Continue..");
			getchar();
			break;
		case '2':
			enterCarInfo(&car);

			if (searchArchive(archiveFp, car.id, ID_COL, 0) == 0)
			{
				operation.lineNum = 0;
				operation.opType = WRITE;
				printf("\n\n\tEntry Created Successfully!");
				getchar();getchar();
			}
			else
			{
				printf("\n\tError Entry with ID '%s' already Exists..", car.id);
				getchar();getchar();
			}
			break;
		case '3':
			printf("\n\n\t[*] Remove Car's Entry [*]\n\n");

			tmpPtr = findAllNumeric(archiveFp, ID_COL);
			printf("\tEntrys (ID): %s", tmpPtr);
			free(tmpPtr);
			tmpPtr = NULL;

			printf("\n\n\tEnter car's ID number: ");
			fgets(car.id, sizeof(car.id), stdin);

			operation.lineNum = searchArchive(archiveFp, car.id, ID_COL, 0);
			if (operation.lineNum == 0)
			{
				getchar();
				operation.opType = INVALID;
				printf("\n\tEntry ID '%s' doesn't exist..", car.id);
				getchar();
			}
			else
			{
				operation.opType = REMOVE;
				printf("\n\tEntry Removed Successfully!");
				getchar();getchar();
			}
			break;
		case '4':
			printf("\n\n\t[*] Modify Car's Entry [*]\n\n");

			tmpPtr = findAllNumeric(archiveFp, ID_COL);
			printf("\tEntrys (ID): %s", tmpPtr);
			free(tmpPtr);
			tmpPtr = NULL;

			printf("\n\n\tEnter car's ID number: ");
			fgets(car.id, sizeof(car.id), stdin);

			operation.lineNum = searchArchive(archiveFp, car.id, ID_COL, 0);
			if (operation.lineNum == 0)
			{
				getchar();
				operation.opType = INVALID;
				printf("\n\tEntry ID '%s' doesn't exist..", car.id);
				getchar();
			}
			else
			{
				clearScreen();
				printf("\n\n\t\t[*] Enter New Entry's Info [*]\n\n");
				printf("\t _______________________________________________________________\n");
				printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
				printf("\t|---------------------------------------------------------------|");
				displayEntry(archiveFp, operation.lineNum);
				printf("\n\t|_______________________________________________________________|");
				getchar();
				
				int tmp = atoi(car.id);
				enterCarInfo(&car);
				rewind(archiveFp);
				if (searchArchive(archiveFp, car.id, ID_COL, 0) == 0 || tmp == atoi(car.id))
				{
					operation.opType = WRITE;
					printf("\n\n\tEntry Modified Successfully!");
					getchar();getchar();
				}
				else
				{
					operation.opType = INVALID;
					printf("\n\tError Entry with ID '%s' already Exists..", car.id);
					getchar();getchar();
				}
			}
			break;
		case '5':
			SEARCH_MENU:
			rewind(archiveFp);
			printSearchMenu();
			scanf("%c", &option);

			if (option == '0')
			{
				getchar();
				break;
			}
			else if (option == '1')
			{
				tmpPtr = findAllNumeric(archiveFp, ID_COL);
				printf("\n\tEntrys (ID): %s", tmpPtr);
			}
			else if (option == '2')
			{
				tmpPtr = findAllAlphaNumeric(archiveFp, COLOR_COL);
				printf("\n\tColors: %s", tmpPtr);
			}
			else if (option == '3')
			{
				tmpPtr = findAllAlphaNumeric(archiveFp, MANUFACT_COL);
				printf("\n\tManufacturers: %s", tmpPtr);
			}
			else if (option == '4')
			{
				tmpPtr = findAllNumeric(archiveFp, DATE_COL);
				printf("\n\tDates: %s", tmpPtr);
			}
			else
				goto SEARCH_MENU;

			free(tmpPtr);
			tmpPtr = NULL;

			getchar();
			printf("\n\n\tEnter Value to Search for: ");
			fgets(searchKey, sizeof(searchKey), stdin);

			/* Remove Line Feed Character */
			if (searchKey[strlen(searchKey)-1] == '\n') 
				searchKey[strlen(searchKey)-1] = '\0';
			
			size_t i; /* Make String Uppercase */
			for (i = 0; i < strlen(searchKey); i++) 
				searchKey[i] = toupper(searchKey[i]);

			/* Test if provided value is valid */
			rewind(archiveFp);
			operation.lineNum = searchArchive(archiveFp, searchKey, 
											  option, searchKey[0]);
			if (operation.lineNum == 0)
				printf("\n\tInvalid Search Key '%s'!", searchKey);
			else /* Print Matching Entrys */
			{
                clearScreen();
				printf("\t _______________________________________________________________\n");
				printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
				printf("\t|---------------------------------------------------------------|");
				do {
					displayEntry(archiveFp, operation.lineNum);
					operation.lineNum = searchArchive(archiveFp, searchKey, option, searchKey[0]);
				} while(operation.lineNum != 0);
				printf("\n\t|_______________________________________________________________|");
			}
			printf("\n\n\tPress Any Key To Continue..");
			getchar();
			goto SEARCH_MENU;
		case '6':
			printf("\n\n\tUpdating '%s' Archive..", ARCHIVE);

			if (operation.opType == WRITE)
			{
				if (operation.lineNum == 0) /* append line */
				{
					fseek(archiveFp, -1, SEEK_END);
					fprintf(archiveFp, "%s %s %s %s\n\n", car.id, car.color, car.manufact, car.date);
					printf("\n\n\tUpdated Archive Successfully!");
				}
				else // modify line
				{
					if (writeToArchive(archiveFp, operation.lineNum, &car) == FALSE) // adds linefeed to the end of the file
						printf("\n\n\tUpdate Failed :(");
					else
						printf("\n\n\tUpdated Archive Successfully!");
				}
			}
			else if (operation.opType == REMOVE)
			{
				if (writeToArchive(archiveFp, operation.lineNum, NULL) == FALSE) // adds linefeed to the end of the file
					printf("\n\n\tUpdate Failed :(");
				else
					printf("\n\n\tUpdated Archive Successfully!");
			}
			
			car = createNullCar();
			getchar();
			break;

		case '0':
			fclose(archiveFp);
			cont = FALSE;
		}
	} while (cont);

	return 0;
}

void printMainMenu(void)
{
	clearScreen();
	printf("\n\n\t[*] Main Menu [*]");
	printf("\n\n\t[1] - View All Entrys");
	printf("\n\t[2] - Create New Entry");
	printf("\n\t[3] - Remove Entry");
	printf("\n\t[4] - Modify Entry");
	printf("\n\t[5] - Search Archive");
	printf("\n\t[6] - Update Archive");
	printf("\n\t[0] - Exit");
	printf("\n\n\tSelect an Option (0-6): ");
}

void printSearchMenu(void)
{
	clearScreen();
	printf("\n\n\t[*] Search Archive [*]");
	printf("\n\n\t[1] - Search By ID");
	printf("\n\t[2] - Search By Color");
	printf("\n\t[3] - Search By Manufacturer");
	printf("\n\t[4] - Search By Date");
	printf("\n\t[0] - Back to Main Menu");
	printf("\n\n\tSelect an Option (0-4): ");
}

void fatal(char* errMsg)
{
	fprintf(stderr, "\n\tFailed While %s", errMsg);
	perror("\n\tERROR");
	printf("\n\tPress Any Key to Exit..");
	getchar();
	
	clearScreen();
	exit(EXIT_FAILURE);
}

void clearScreen(void)
{
	printf("\033[H\033[2J");
}

Car createNullCar(void)
{
	Car car;

	car.id[0] = '\0';
	car.color[0] = '\0';
	car.manufact[0] = '\0';
	car.date[0] = '\0';

	return car;
}

void enterCarInfo(Car* car)
{
	boolean invalidInput;
	size_t i;

	do {
		invalidInput = FALSE;
		printf("\n\n\tEnter Entry's ID (%d..%d): ", MIN_ID, MAX_ID);
		fgets(car->id, sizeof(car->id), stdin);

		for(i = 0; i < strlen(car->id); i++)
			if (!isdigit(*(car->id + i)))
				invalidInput = TRUE;

		if (!invalidInput && (atoi(car->id) < MIN_ID || atoi(car->id) > MAX_ID))
			invalidInput = TRUE;

		if (invalidInput)
		{
			printf("\n\tInvalid ID Provided!");
			printf("\n\tAcceptable IDs are between %d and %d.", MIN_ID, MAX_ID);
        }
        fflush(stdin);
	} while (invalidInput);

	getchar();
	printf("\n\n\tEnter Car's Color: ");
	fgets(car->color, sizeof(car->color), stdin);
    fflush(stdin);
    
	printf("\n\n\tEnter Car's Manufacturer: ");
	fgets(car->manufact, sizeof(car->manufact), stdin);
    fflush(stdin);
    
	/* Remove Leading Line Feed & Uppercase String */
	if(car->color[strlen(car->color)-1] == '\n')
		car->color[strlen(car->color)-1] = '\0';
	for (i = 0; i < strlen(car->color); i++)
		car->color[i] = toupper(car->color[i]);

	/* Remove Leading Line Feed & Uppercase String */
	if(car->manufact[strlen(car->manufact)-1] == '\n')
		car->manufact[strlen(car->manufact)-1] = '\0';
	for (i = 0; i < strlen(car->manufact); i++)
		car->manufact[i] = toupper(car->manufact[i]);

	do {
		invalidInput = FALSE;
		printf("\n\n\tEnter car's date of Manufacture (%d..%d): ", MIN_DATE, MAX_DATE);
		fgets(car->date, sizeof(car->date), stdin);

		for(i = 0; i < strlen(car->date); i++)
			if (isdigit(*(car->date + i)) == 0)
				invalidInput = TRUE;

		if (!invalidInput && (atoi(car->date) < MIN_DATE || atoi(car->date) > MAX_DATE))
			invalidInput = TRUE;

		if (invalidInput)
		{
			printf("\n\tInvalid Date Provided!");
			printf("\n\tAcceptable Dates are between %d and %d.", MIN_DATE, MAX_DATE);
		}
		fflush(stdin);
	} while (invalidInput);
}

void displayEntrys(FILE* fp)
{
	Car car = createNullCar();
	
	rewind(fp);
	printf("\t _______________________________________________________________\n");
	printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
	printf("\t|---------------------------------------------------------------|\n");
	while (!feof(fp))
	{
		fscanf(fp, "%s %s %s %s\n", car.id, car.color, car.manufact, car.date);
		if (strlen(car.manufact) < 8)
			strcat(car.manufact, "\t");
		printf("\t| %s\t\t%s\t\t%s\t\t%s\t|\n", car.id, car.color, car.manufact, car.date);
	}
	printf("\t|_______________________________________________________________|");
}

size_t searchArchive(FILE* fp, char* key, char col, char sign)
{
	static size_t lineCount = 0;
	Car car;

	if ((col == ID_COL || col == DATE_COL) && sign != '\0')
		key++;

	while(!feof(fp))
	{
		lineCount++;
		fscanf(fp, "%s %s %s %s\n", car.id, car.color, car.manufact, car.date);

		switch (col)
		{
		case ID_COL:
			if ((sign == '>' && atoi(car.id) > atoi(key)) ||
				(sign == '<' && atoi(car.id) < atoi(key)) ||
				(sign == '\0' && !strcmp(car.id, key)))
				return lineCount;
			break;
		case COLOR_COL:
			if (strncmp(key, car.color, strlen(key)) == 0) return lineCount;
			break;
		case MANUFACT_COL:
			if (strncmp(key, car.manufact, strlen(key)) == 0) return lineCount;
			break;
		case DATE_COL:
			if ((sign == '>' && atoi(car.date) > atoi(key)) ||
				(sign == '<' && atoi(car.date) < atoi(key)))
				return lineCount;
		}
	}
	
	lineCount = 0;
	return lineCount;
}

void displayEntry(FILE* fp, size_t lineNum)
{
	Car car = createNullCar();
	int i;

	rewind(fp);
	for (i = 1; i < lineNum; ++i)
		fscanf(fp, "%s %s %s %s\n", car.id, car.color, car.manufact, car.date);

	fscanf(fp, "%s %s %s %s\n", car.id, car.color, car.manufact, car.date);
	if (strlen(car.manufact) < 8)
		strcat(car.manufact, "\t");
	
	printf("\n\t| %s\t\t%s\t\t%s\t\t%s\t|", car.id, car.color, car.manufact, car.date);
}

char* findAllNumeric(FILE* fp, char col)
{
	size_t i, lineCount = 0;
	char tmpBuf[10];
	Car car;
		
	lineCount = getLineCount(fp);

	int entrys[lineCount];
	for (i = 0; i < lineCount; i++)
	{
		fscanf(fp, "%s %s %s %s\n", car.id, car.color, car.manufact, car.date);
		entrys[i] = (col == ID_COL) ? atoi(car.id) : atoi(car.date);
	}
	quicksort(&entrys[0], 0, lineCount-1);

	char* buf = (char*) malloc((lineCount * 6) + 1);
	if (buf == NULL)
		fatal("Allocating Memory!");
	
	snprintf(tmpBuf, 5, "%d", entrys[0]);
	for (i = 1; i < lineCount; ++i)
	{
		strcat(buf, tmpBuf);
		snprintf(tmpBuf, 7, ", %d", entrys[i]);
	}
	strcat(buf, tmpBuf);

	rewind(fp);
	return buf;
}

char* findAllAlphaNumeric(FILE* fp, char col)
{
	size_t i, lineCount = 0;
	Car car;

	lineCount = getLineCount(fp);

	char** arr = malloc(lineCount * sizeof(char*));
	if (arr == NULL)
		fatal("Allocating Memory!");
	
	for (i = 0; i < lineCount; i++)
	{
	    arr[i] = (char *) malloc(sizeof(char) * 30);
	    if (arr[i] == NULL)
	  	    fatal("Allocating Memory!");

		fscanf(fp, "%s %s %s %s\n", car.id, car.color, car.manufact, car.date);
		col == COLOR_COL ? strcpy(arr[i], car.color) : strcpy(arr[i], car.manufact);
	}
	i = removeDuplicate(&arr[0], lineCount);

	char* buf = (char *) malloc(lineCount * 32);
	if (buf == NULL)
		fatal("Allocating Memory!");
	
	strcpy(buf, arr[0]);
	for (; i > 0; i--)
	{
		strcat(buf, ", ");
		strcat(buf, arr[i]);
		
		free(arr[i]);
		arr[i] = NULL;
	}
	free(arr);
	arr = NULL;
	
	rewind(fp);
	return buf;
}

size_t getLineCount(FILE* fp)
{
	size_t lineCount = 0;
	char ch;
	
	rewind(fp);
	while(!feof(fp))
	{
		ch = fgetc(fp);
		if(ch == '\n')
			lineCount++;
	}
	
	rewind(fp);
	return lineCount-1;
}

boolean writeToArchive(FILE* fp, size_t lineNum, Car* car)
{
	char buf[62];
	FILE* tmpFp;
	int ret;

	tmpFp = fopen(TMP_FILE, "w");
	if (tmpFp == NULL)
        fatal("opening Temporary File!");

	rewind(fp);
	for(; lineNum > 1; --lineNum)
	{
		fgets(buf, sizeof(buf), fp);
		if (strcmp(buf, "\n"))
			fputs(buf, tmpFp);
	}

	if (car != NULL)
		fprintf(tmpFp, "%s %s %s %s\n", car->id, car->color, car->manufact, car->date);
	fgets(buf, sizeof(buf), fp); 

	while(!feof(fp))
	{
		fgets(buf, sizeof(buf), fp);
		if (strcmp(buf, "\n"))
			fputs(buf, tmpFp);
	}
	fputc('\n', tmpFp);
	fclose(tmpFp);
	fclose(fp);

	ret = remove(ARCHIVE);
	if (ret == -1)
		fatal("Removing Outdated Archive!");

	ret = rename(TMP_FILE, ARCHIVE);
	if (ret == -1)
		fatal("Renaming Archive!");

	fp = fopen(ARCHIVE, "r+");
	if (fp == NULL)
		fatal("Reopening Archive!");

	return TRUE;
}
