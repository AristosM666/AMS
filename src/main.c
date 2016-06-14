#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "algrtms.h"

const char ARCHIVE[] = "ams_archive.csv";
const char TMP_FILE[] = "tmp.csv";

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
	size_t entryNum;
	enum { INVALID,	REMOVE,	WRITE } opType;
} Operation;

/* Returns a string of all UNIQUE numeric values in the DATE or ID fields */
char* findAllNumeric(FILE* fp, char col);
/* Returns all UNIQUE alpha-numeric strings in the COLOR or MANUFACT fields */
char* findAllAlphaNumeric(FILE* fp, char col);
/* Returns the entry number of the first matching entry */
size_t searchArchive(FILE* fp, char* key, char col, char sign);
/* Overwrites/Removes archive entry */
boolean writeToArchive(FILE* fp, size_t entryNum, Car* car);
void displayEntry(FILE* fp, size_t entryNum); 
void displayEntrys(FILE* fp);
/* Get entry count from archive */
size_t getEntryCount(FILE* fp);
/* Initialize a Car with empty strings and return it */
Car createNullCar(void);
void enterCarInfo(Car* car);
/* display error message and exit */
void fatal(char* errMsg);
void clearScreen(void);
void displaySplashScreen(void);
void displayMainMenu(void);
void displaySearchMenu(void);
boolean readEntry(FILE* fp, Car* car);

int main(void)
{
	char option, searchKey[30], *tmpPtr;
	size_t i; 
	boolean cont = TRUE;
	FILE* archiveFp;
	Operation operation;
	Car car;
	
	displaySplashScreen();

	archiveFp = fopen(ARCHIVE, "r+");
	if (archiveFp == NULL)
		fatal("opening archive");

	do {
		car = createNullCar();
		displayMainMenu();
		fflush(stdin);
		scanf("%c", &option);
		getchar(); /* <-------------------------------------------------- */

		clearScreen();

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
			getchar(); /* <-------------------------------------------------- */

			if (searchArchive(archiveFp, car.id, ID_COL, 0) == 0)
			{
				operation.entryNum = 0;
				operation.opType = WRITE;
				printf("\n\n\tEntry Created Successfully!");
				getchar();
			}
			else
			{
				printf("\n\tError Entry with ID '%s' already Exists..", car.id);
				getchar();
			}
			break;
		case '3':
			printf("\n\n\t[*] Remove Car's Entry [*]\n\n");

			tmpPtr = findAllNumeric(archiveFp, ID_COL);
			printf("\tEntrys (ID): %s", tmpPtr);
			free(tmpPtr);
			tmpPtr = NULL;

			printf("\n\n\tEnter car's ID number: ");
			fflush(stdin);
			fgets(car.id, sizeof(car.id), stdin);
			getchar(); /* <-------------------------------------------------- */

			operation.entryNum = searchArchive(archiveFp, car.id, ID_COL, 0);
			if (operation.entryNum == 0)
			{
				operation.opType = INVALID;
				printf("\n\tEntry ID '%s' doesn't exist..", car.id);
				getchar();
			}
			else
			{
				operation.opType = REMOVE;
				printf("\n\tEntry Removed Successfully!");
				getchar();
			}
			break;
		case '4':
			printf("\n\n\t[*] Modify Car's Entry [*]\n\n");

			tmpPtr = findAllNumeric(archiveFp, ID_COL);
			printf("\tEntrys (ID): %s", tmpPtr);
			free(tmpPtr);
			tmpPtr = NULL;

			printf("\n\n\tEnter car's ID number: ");
			fflush(stdin);
			fgets(car.id, sizeof(car.id), stdin);
			getchar(); /* <-------------------------------------------------- */

			operation.entryNum = searchArchive(archiveFp, car.id, ID_COL, 0);
			if (operation.entryNum == 0)
			{
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
				displayEntry(archiveFp, operation.entryNum);
				printf("\n\t|_______________________________________________________________|");
				
				i = atoi(car.id);
				enterCarInfo(&car);
				getchar(); /* <-------------------------------------------------- */
				if (searchArchive(archiveFp, car.id, ID_COL, 0) == 0 || i == atoi(car.id))
				{
					operation.opType = WRITE;
					printf("\n\n\tEntry Modified Successfully!");
					getchar();
				}
				else
				{
					operation.opType = INVALID;
					printf("\n\tError Entry with ID '%s' already Exists..", car.id);
					getchar();
				}
			}
			break;
		case '5':
			SEARCH_MENU:
			displaySearchMenu();
			fflush(stdin);
			scanf("%c", &option);

			if (option == '0')
			{
				getchar(); /* <-------------------------------------------------- */
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

			getchar();  /* <-------------------------------------------------- */
			printf("\n\n\tEnter Search Term: ");
			fflush(stdin);
			fgets(searchKey, sizeof(searchKey), stdin);

			/* Remove Line Feed Character */
			if (searchKey[strlen(searchKey)-1] == '\n') 
				searchKey[strlen(searchKey)-1] = '\0';
			
			/* Make String Uppercase */
			for (i = 0; i < strlen(searchKey); i++) 
				searchKey[i] = toupper(searchKey[i]);

			/* Test if provided value is valid */
			rewind(archiveFp);
			operation.entryNum = searchArchive(archiveFp, searchKey, 
											  option, searchKey[0]);
			if (operation.entryNum == 0)
				printf("\n\tInvalid Search Key '%s'!", searchKey);
			else /* Print Matching Entrys */
			{
                clearScreen();
                printf("\n\n\t[*] Search Results [*]\n");
				printf("\t _______________________________________________________________\n");
				printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
				printf("\t|---------------------------------------------------------------|");
				do {
					displayEntry(archiveFp, operation.entryNum);
					operation.entryNum = searchArchive(archiveFp, searchKey, option, searchKey[0]);
				} while(operation.entryNum != 0);
				printf("\n\t|_______________________________________________________________|");
			}
			printf("\n\n\tPress Any Key To Continue..");
			getchar();
			goto SEARCH_MENU;
		case '6':
			printf("\n\n\tUpdating '%s' Archive..", ARCHIVE);

			if (operation.opType == WRITE)
			{
				if (operation.entryNum == 0) /* append entry */
				{
					fseek(archiveFp, -1, SEEK_END);
					fprintf(archiveFp, "%s,%s,%s,%s,", car.id, car.color, car.manufact, car.date);
					fflush(archiveFp);
					printf("\n\n\tUpdated Archive Successfully!");
				}
				else /* modify entry */
				{
					if (writeToArchive(archiveFp, operation.entryNum, &car) == FALSE)
						printf("\n\n\tUpdate Failed :(");
					else
						printf("\n\n\tUpdated Archive Successfully!");
				}
			}
			else if (operation.opType == REMOVE)
			{
				if (writeToArchive(archiveFp, operation.entryNum, NULL) == FALSE)
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

void displaySplashScreen(void)
{
	system("TITLE Automotive Managment System");

	clearScreen();
	printf("\n\n\n\n\t\t[::] Automotive Management System [::]\n\n");
	printf("\t\tCollege Software Engineering Project.\n\n");
	printf("\t\tWritten By:\tAristos Miliaressis\n");
	printf("\t\t\tAt:\t10-10-2015\n");
	printf("\t\t\tIn:\tANSI C\n\n");
	printf("\t\tPress Any Key to Continue...");
	fflush(stdin);
	getchar();
}

void displayMainMenu(void)
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

void displaySearchMenu(void)
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
	fprintf(stderr, "\n\n\t\tFailed while %s!", errMsg);
	perror("\n\t\tERROR");
	printf("\n\t\tPress Any Key to Exit..");
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
		fflush(stdin);
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
	} while (invalidInput);

	getchar(); /* <-------------------------------------------------- */
	printf("\n\n\tEnter Car's Color: ");
	fflush(stdin);
	fgets(car->color, sizeof(car->color), stdin);
    
	printf("\n\n\tEnter Car's Manufacturer: ");
	fflush(stdin);
	fgets(car->manufact, sizeof(car->manufact), stdin);
    
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
		fflush(stdin);
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
	} while (invalidInput);
}

void displayEntrys(FILE* fp)
{
	Car car = createNullCar();
	
	printf("\t _______________________________________________________________\n");
	printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
	printf("\t|---------------------------------------------------------------|\n");
	rewind(fp);
	while (readEntry(fp, &car))
	{
		if (strlen(car.manufact) < 8)
			strcat(car.manufact, "\t");
		printf("\t| %s\t\t%s\t\t%s\t\t%s\t|\n", car.id, car.color, car.manufact, car.date);
		car = createNullCar();
	}
	printf("\t|_______________________________________________________________|");
}

boolean readEntry(FILE* fp, Car* car)
{
	if (!freadUntilDelim(fp, ',', &car->id[0]))
		return FALSE;
	else if (!freadUntilDelim(fp, ',', &car->color[0]))
		return FALSE;
	else if (!freadUntilDelim(fp, ',', &car->manufact[0]))
		return FALSE;
	else if (!freadUntilDelim(fp, ',', &car->date[0]))
		return FALSE;
	else
		return TRUE;
}

size_t searchArchive(FILE* fp, char* key, char col, char sign)
{
	static size_t entryCount = 0;
	Car car;

	if ((col == ID_COL || col == DATE_COL) && sign != '\0')
		key++;

	while(!feof(fp))
	{
		entryCount++;
		readEntry(fp, &car);

		switch (col)
		{
		case ID_COL:
			if ((sign == '>' && atoi(car.id) > atoi(key)) ||
				(sign == '<' && atoi(car.id) < atoi(key)) ||
				(sign == '\0' && !strcmp(car.id, key)))
				return entryCount;
			break;
		case COLOR_COL:
			if (strncmp(key, car.color, strlen(key)) == 0) return entryCount;
			break;
		case MANUFACT_COL:
			if (strncmp(key, car.manufact, strlen(key)) == 0) return entryCount;
			break;
		case DATE_COL:
			if ((sign == '>' && atoi(car.date) > atoi(key)) ||
				(sign == '<' && atoi(car.date) < atoi(key)))
				return entryCount;
		}
	}
	
	entryCount = 0;
	return entryCount;
}

void displayEntry(FILE* fp, size_t entryNum)
{
	Car car = createNullCar();
	int i;

	rewind(fp);
	for (i = 1; i <= entryNum; ++i)
		readEntry(fp, &car);

	if (strlen(car.manufact) < 8)
		strcat(car.manufact, "\t");
	
	printf("\n\t| %s\t\t%s\t\t%s\t\t%s\t|", car.id, car.color, car.manufact, car.date);
}

char* findAllNumeric(FILE* fp, char col)
{
	const size_t entryCount = getEntryCount(fp);
	char* buf = (char*) malloc((entryCount * 6) + 1);
	int* entrys = (int*) malloc(entryCount * sizeof(int));
	char tmpBuf[10];
	size_t i;
	Car car;
	
	if (buf == NULL)
		fatal("allocating memory");

	rewind(fp);
	for (i = 0; i < entryCount; i++)
	{
		readEntry(fp, &car);
		entrys[i] = (col == ID_COL) ? atoi(car.id) : atoi(car.date);
	}
	quicksort(&entrys[0], 0, entryCount-1);
	
	snprintf(tmpBuf, 5, "%d", entrys[0]);
	for (i = 1; i < entryCount; ++i)
	{
		strcat(buf, tmpBuf);
		snprintf(tmpBuf, 7, ", %d", entrys[i]);
	}
	strcat(buf, tmpBuf);
	free(entrys);

	return buf;
}

char* findAllAlphaNumeric(FILE* fp, char col)
{
	const size_t entryCount = getEntryCount(fp);
	char** arr = (char**) malloc(entryCount * sizeof(char*));
	char* buf = (char*) malloc(entryCount * 32);
	size_t i;
	Car car;

	if (arr == NULL || buf == NULL)
		fatal("allocating memory");
	
	rewind(fp);
	for (i = 0; i < entryCount; i++)
	{
	    arr[i] = (char *) malloc(sizeof(char) * 30);
	    if (arr[i] == NULL)
	  	    fatal("allocating memory");

		readEntry(fp, &car);
		col == COLOR_COL ? strcpy(arr[i], car.color) : strcpy(arr[i], car.manufact);
	}
	i = removeDuplicate(&arr[0], entryCount);
	
	strcpy(buf, arr[0]);
	for (; i > 0; i--)
	{
		strcat(buf, ", ");
		strcat(buf, arr[i]);
		
		free(arr[i]);
		arr[i] = NULL;
	}
	free(arr[0]);
	free(arr);
	arr = NULL;
	
	return buf;
}

size_t getEntryCount(FILE* fp)
{
	size_t entryCount = 0;
	char ch;
	
	fflush(fp);
	rewind(fp);
	while(!feof(fp))
	{
		ch = fgetc(fp);
		if(ch == ',')
			entryCount++;
	}
	
	return entryCount / 4;
}

boolean writeToArchive(FILE* fp, size_t entryNum, Car* car)
{
	FILE* tmpFp;
	Car tmpCar = createNullCar();
	int ret;

	tmpFp = fopen(TMP_FILE, "w");
	if (tmpFp == NULL)
        fatal("opening temporary file");

	rewind(fp);
	for(; entryNum > 1; --entryNum)
	{
		readEntry(fp, &tmpCar);
		fprintf(tmpFp, "%s,%s,%s,%s,", tmpCar.id, tmpCar.color, tmpCar.manufact, tmpCar.date);
		fflush(tmpFp);
	}

	if (car != NULL)
	{
		fprintf(tmpFp, "%s,%s,%s,%s,", car->id, car->color, car->manufact, car->date);
		fflush(tmpFp);
	}
	readEntry(fp, &tmpCar);

	while(readEntry(fp, &tmpCar))
	{
		fprintf(tmpFp, "%s,%s,%s,%s,", tmpCar.id, tmpCar.color, tmpCar.manufact, tmpCar.date);
		fflush(tmpFp);
	}
	fclose(tmpFp);
	fclose(fp);

	ret = remove(ARCHIVE);
	if (ret == -1)
		fatal("removing outdated archive");

	ret = rename(TMP_FILE, ARCHIVE);
	if (ret == -1)
		fatal("renaming archive");

	fp = fopen(ARCHIVE, "r+");
	if (fp == NULL)
		fatal("reopening archive");

	return TRUE;
}
