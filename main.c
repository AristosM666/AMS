#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "algrtms.h"

typedef enum { FALSE, TRUE } boolean;

const char ARCHIVE[] = "cars.txt";
const char TMP_FILE[] = "tmp.txt";

#define ID_COL '1'
#define COLOR_COL '2'
#define MANUFACT_COL '3'
#define DATE_COL '4'
const unsigned short MIN_DATE = 1960;
const unsigned short MAX_DATE = 2006;
const unsigned short MIN_ID = 1000;
const unsigned short MAX_ID = 9999;
typedef struct {
	char id[5];
	char color[30];
	char manufact[30];
	char date[5];
} CarStruct;

typedef struct {
	unsigned int lnum;
	enum { INVALID,	REMOVE,	WRITE } fileAction;
} OperationStruct;

/* Returns the line number of the first matching entry */
unsigned int searchArchive(FILE*, char*, char, char); 
/* Overwrites/Removes archive entry */
boolean writeEntry(FILE*, unsigned int, CarStruct*); 
void printEntry(FILE*, unsigned int); 
void displayArchive(FILE*);
void createEntryStruct(CarStruct*);
/* Returns a string of all UNIQUE numeric values in the DATE or ID fields */
char* findAllNumeric(FILE*, char); 
/* Returns a string of all UNIQUE alpha-numeric values in the COLOR or MANUFACT fields */
char* findAllAlphaNumeric(FILE*, char); 


void fatal(char*); /* display error message and exit */

int main(void)
{
	OperationStruct operationInstance;
	CarStruct carInstance;
	boolean cont = TRUE;
	FILE* archiveFp;
	char option;
	
	system("TITLE Automotive Managment System");

	archiveFp = fopen(ARCHIVE, "r+");
	if (archiveFp == NULL)
		fatal("Opening Archive!");

	do {
		system("CLS");
		printf("\n\n\t[*] Main Menu [*]");
		printf("\n\n\t[1] - View All Entrys");
		printf("\n\t[2] - Create New Entry");
		printf("\n\t[3] - Remove Entry");
		printf("\n\t[4] - Modify Entry");
		printf("\n\t[5] - Search Archive");
		printf("\n\t[6] - Update Archive");
		printf("\n\t[0] - Exit");
		printf("\n\n\tSelect an Option (0-6): ");
		scanf("%c", &option);

		system("CLS");
		rewind(archiveFp);
		getchar();

		switch (option)
		{
		case '1':
			printf("\n\n\t\t[*] View Archive Entrys [*]\n\n");
			displayArchive(archiveFp);

			printf("\n\n\t\tPress Any Key To Continue..");
			getchar();
			break;
		case '2':
			createEntryStruct(&carInstance);

			if (searchArchive(archiveFp, carInstance.id, ID_COL, 0) == 0)
			{
				operationInstance.lnum = 0;
				operationInstance.fileAction = WRITE;
				printf("\n\n\tEntry Created Successfully!");
				getchar();getchar();
			}
			else
			{
				printf("\n\tError Entry with ID '%s' already Exists..", carInstance.id);
				getchar();getchar();
			}
			break;
		case '3':
			char* tmpPtr;
			printf("\n\n\t[*] Remove Car's Entry [*]\n\n");

			tmpPtr = findAllNumeric(archiveFp, ID_COL);
			printf("\tEntrys (ID): %s", tmpPtr);
			free(tmpPtr);

			printf("\n\n\tEnter car's ID number: ");
			fgets(carInstance.id, sizeof(carInstance.id), stdin);

			operationInstance.lnum = searchArchive(archiveFp, carInstance.id, ID_COL, 0);
			if (operationInstance.lnum == 0)
			{
				getchar();
				operationInstance.fileAction = INVALID;
				printf("\n\tEntry ID '%s' doesn't exist..", carInstance.id);
				getchar();
			}
			else
			{
				operationInstance.fileAction = REMOVE;
				printf("\n\tEntry Removed Successfully!");
				getchar();getchar();
			}
			break;
		case '4':
			unsigned short tmp;
			char* tmpPtr;
			printf("\n\n\t[*] Modify Car's Entry [*]\n\n");

			tmpPtr = findAllNumeric(archiveFp, ID_COL);
			printf("\tEntrys (ID): %s", tmpPtr);
			free(tmpPtr);

			printf("\n\n\tEnter car's ID number: ");
			fgets(carInstance.id, sizeof(carInstance.id), stdin);
			tmp = atoi(carInstance.id);

			operationInstance.lnum = searchArchive(archiveFp, carInstance.id, ID_COL, 0);
			if (operationInstance.lnum == 0)
			{
				getchar();
				operationInstance.fileAction = INVALID;
				printf("\n\tEntry ID '%s' doesn't exist..", carInstance.id);
				getchar();
			}
			else
			{
				system("CLS");
				printf("\n\n\t\t[*] Enter New Entry's Info [*]\n\n");
				printf("\t _______________________________________________________________\n");
				printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
				printf("\t|---------------------------------------------------------------|");
				printEntry(archiveFp, operationInstance.lnum);
				printf("\n\t|_______________________________________________________________|");
				getchar();
				createEntryStruct(&carInstance);
				rewind(archiveFp);
				if (searchArchive(archiveFp, carInstance.id, ID_COL, 0) == 0 || tmp == atoi(carInstance.id))
				{
					operationInstance.fileAction = WRITE;
					printf("\n\n\t\tEntry Modified Successfully!");
					getchar();getchar();
				}
				else
				{
					operationInstance.fileAction = INVALID;
					printf("\n\tError Entry with ID '%s' already Exists..", carInstance.id);
					getchar();getchar();
				}
			}
			break;
		case '5':
			char searchKey[30], *tmpPtr;
			int i;
			SEARCH_MENU:
			rewind(archiveFp);
			system("CLS");
			printf("\n\n\t[*] Search Archive [*]");
			printf("\n\n\t[1] - Search By ID");
			printf("\n\t[2] - Search By Color");
			printf("\n\t[3] - Search By Manufacturer");
			printf("\n\t[4] - Search By Date");
			printf("\n\t[0] - Back to Main Menu");
			printf("\n\n\tSelect an Option (0-4): ");
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

			getchar();
			printf("\n\n\tEnter Value to Search for: ");
			fgets(searchKey, sizeof(searchKey), stdin);

			if (searchKey[strlen(searchKey)-1] == '\n') /* Remove Line Feed Character */
				searchKey[strlen(searchKey)-1] = '\0';
			for (i = 0; i < strlen(searchKey); i++) /* Make String Uppercase */
				searchKey[i] = toupper(searchKey[i]);

			rewind(archiveFp);
			operationInstance.lnum = searchArchive(archiveFp, searchKey, option, searchKey[0]); /* Test if provided value is valid */
			if (operationInstance.lnum == 0)
				printf("\n\tInvalid Search Key '%s'!", searchKey);
			else /* Print Matching Entrys */
			{
                system("CLS");
				printf("\t _______________________________________________________________\n");
				printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
				printf("\t|---------------------------------------------------------------|");
				do {
					printEntry(archiveFp, operationInstance.lnum);
					operationInstance.lnum = searchArchive(archiveFp, searchKey, option, searchKey[0]);
				} while(operationInstance.lnum != 0);
				printf("\n\t|_______________________________________________________________|");
			}
			printf("\n\n\tPress Any Key To Continue..");
			getchar();
			goto SEARCH_MENU;
		case '6':
			printf("\n\n\tUpdating '%s' Archive..", ARCHIVE);

			if (operationInstance.fileAction == WRITE)
			{
				if (operationInstance.lnum == 0) /* append line */
				{
					fseek(archiveFp, 0, SEEK_END);
					fprintf(archiveFp, "\n%s %s %s %s", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
					printf("\n\n\tUpdated Archive Successfully!");
				}
				else
				{
					if (writeEntry(archiveFp, operationInstance.lnum, &carInstance) == FALSE)
						printf("\n\n\tUpdate Failed :(");
					else
						printf("\n\n\tUpdated Archive Successfully!");
				}
			}
			else if (operationInstance.fileAction == REMOVE)
			{
				if (writeEntry(archiveFp, operationInstance.lnum, NULL) == FALSE)
					printf("\n\n\tUpdate Failed :(");
				else
					printf("\n\n\tUpdated Archive Successfully!");
			}
			getchar();
			break;
		case '0':
			fclose(archiveFp);
			cont = FALSE;
		}
	} while (cont);

	return 0;
}

void fatal(char* errMsg)
{
	fprintf(stderr, "\n\tFailed While %s", errMsg);
	perror("\n\tERROR");
	printf("\n\tPress Any Key to Exit..");
	getchar();
	exit(EXIT_FAILURE);
}

void createEntryStruct(CarStruct* ptrCarInstance)
{
	boolean invalidInput;
	int i;

	do {
		invalidInput = FALSE;
		printf("\n\n\tEnter Entry's ID (1000-9999): ");
		fgets(ptrCarInstance->id, sizeof(ptrCarInstance->id), stdin);

		for(i = 0; i < strlen(ptrCarInstance->id); i++)
			if (isdigit(*(ptrCarInstance->id + i)) == 0)
				invalidInput = TRUE;

		if (!invalidInput && (atoi(ptrCarInstance->id) < MIN_ID || atoi(ptrCarInstance->id) > MAX_ID))
			invalidInput = TRUE;

		if (invalidInput)
		{
			printf("\n\tInvalid ID Provided!");
			printf("\n\tAcceptable IDs are between %d and %d.", MIN_ID, MAX_ID);
        }
        fflush(stdin);
	} while (invalidInput);

	printf("\n\n\tEnter Car's Color: ");
	fgets(ptrCarInstance->color, sizeof(ptrCarInstance->color), stdin);
    fflush(stdin);
    
	printf("\n\n\tEnter Car's Manufacturer: ");
	fgets(ptrCarInstance->manufact, sizeof(ptrCarInstance->manufact), stdin);
    fflush(stdin);
    
	/* Remove Leading Line Feed & Uppercase String */
	if(ptrCarInstance->color[strlen(ptrCarInstance->color)-1] == '\n')
		ptrCarInstance->color[strlen(ptrCarInstance->color)-1] = '\0';
	for (i = 0; i < strlen(ptrCarInstance->color); i++)
		ptrCarInstance->color[i] = toupper(ptrCarInstance->color[i]);

	/* Remove Leading Line Feed & Uppercase String */
	if(ptrCarInstance->manufact[strlen(ptrCarInstance->manufact)-1] == '\n')
		ptrCarInstance->manufact[strlen(ptrCarInstance->manufact)-1] = '\0';
	for (i = 0; i < strlen(ptrCarInstance->manufact); i++)
		ptrCarInstance->manufact[i] = toupper(ptrCarInstance->manufact[i]);

	do {
		invalidInput = FALSE;
		printf("\n\n\tEnter car's date of Manufacture (1960-2015): ");
		fgets(ptrCarInstance->date, sizeof(ptrCarInstance->date), stdin);

		for(i = 0; i < strlen(ptrCarInstance->date); i++)
			if (isdigit(*(ptrCarInstance->date + i)) == 0)
				invalidInput = TRUE;

		if (!invalidInput && (atoi(ptrCarInstance->date) < MIN_DATE || atoi(ptrCarInstance->date) > MAX_DATE))
			invalidInput = TRUE;

		if (invalidInput)
		{
			printf("\n\tInvalid Date Provided!");
			printf("\n\tAcceptable Dates are between %d and %d.", MIN_DATE, MAX_DATE);
			fflush(stdin);
		}
	} while (invalidInput);
}

void displayArchive(FILE* fp)
{
	CarStruct carInstance;

	rewind(fp);
	printf("\t _______________________________________________________________\n");
	printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
	printf("\t|---------------------------------------------------------------|\n");
	while (!feof(fp))
	{
		fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
		if (strlen(carInstance.manufact) < 8)
			strcat(carInstance.manufact, "\t");
		printf("\t| %s\t\t%s\t\t%s\t\t%s\t|\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
	}
	printf("\t|_______________________________________________________________|");
}

unsigned int searchArchive(FILE* fp, char* key, char col, char sign)
{
	static unsigned int lineCount = 0;
	CarStruct carInstance;

	if ((col == ID_COL || col == DATE_COL) && sign != '\0')
		key++;

	while(!feof(fp))
	{
		lineCount++;
		fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);

		switch (col)
		{
		case ID_COL:
			if ((sign == '>' && atoi(carInstance.id) > atoi(key) ||
				(sign == '<' && atoi(carInstance.id) < atoi(key) ||
				(sign == '\0' && !strcmp(carInstance.id, key))
				return lineCount;
			break;
		case COLOR_COL:
			if (strncmp(key, carInstance.color, strlen(key)) == 0) return lineCount;
			break;
		case MANUFACT_COL:
			if (strncmp(key, carInstance.manufact, strlen(key)) == 0) return lineCount;
			break;
		case DATE_COL:
			if ((sign == '>' && atoi(carInstance.date) > atoi(key) ||
				(sign == '<' && atoi(carInstance.date) < atoi(key))
				return lineCount;
		}
	}
	
	lineCount = 0;
	return 0;
}

void printEntry(FILE* fp, unsigned int lnum)
{
	CarStruct carInstance;
	unsigned int i;

	rewind(fp);
	for (i = 1; i < lnum; i++)
		fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);

	fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
	if (strlen(carInstance.manufact) < 8)
		strcat(carInstance.manufact, "\t");
	
	printf("\n\t| %s\t\t%s\t\t%s\t\t%s\t|", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
}

char* findAllNumeric(FILE* fp, char col)
{
	CarStruct carInstance;
	unsigned int i, lnum = 0;
	char tmpBuf[5];
	
	while(!feof(fp))
	{
		fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
		lnum++;
	}

	rewind(fp);
	unsigned short entrys[lnum];

	for (i = 0; i < lnum; i++)
	{
		fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
		entrys[i] = (unsigned short) (col == ID_COL) ? atoi(carInstance.id) : atoi(carInstance.date);
	}
	quicksort(&entrys[0], 0, lnum);

	char* buf = (char*) malloc((lnum * 6) - 1);
	if (buf == NULL)
		fatal("Allocating Memory!");
	snprintf(tmpBuf, sizeof(tmpBuf)+1, "%d", entrys[lnum]);
	strcpy (buf, tmpBuf);
	
	for (lnum -= 1; lnum < -1; lnum--)
	{
   		snprintf(tmpBuf, sizeof(tmpBuf)+1, "%d", entrys[lnum]);
   		strcat (buf, ", ");
		strcat (buf, tmpBuf);
	}

	rewind(fp);
	return buf;
}

char* findAllAlphaNumeric(FILE* fp, char col)
{
	CarStruct carInstance;
	unsigned int i, lnum = 0;

	while(!feof(fp))
	{
		fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
		lnum++;
	}

	rewind(fp);
	char** arr = malloc(lnum * sizeof(char*));
	if (arr == NULL)
		fatal("Allocating Memory!");
	
	for (i = 0; i < lnum; i++)
	{
	    arr[i] = (char *) malloc(sizeof(char *) * 30);
	    if (arr[i] == NULL)
	  	    fatal("Allocating Memory!");

		fscanf(fp, "%s %s %s %s\n", carInstance.id, carInstance.color, carInstance.manufact, carInstance.date);
		col == COLOR_COL ? strcpy(arr[i], carInstance.color) : strcpy(arr[i], carInstance.manufact);
	}
	i = removeDuplicate(&arr[0], i);

	char* buf = (char *) malloc(lnum * 32);
	if (buf == NULL)
		fatal("Allocating Memory!");
	strcpy(buf, arr[i-1]);
	
	for (i -= 2; i < -1; i--)
	{
		strcat(buf, ", ");
		strcat(buf, arr[i]);
	}
	
	free(arr);
	return buf;
}

boolean writeEntry(FILE* fp, unsigned int lnum, CarStruct* ptrCarInstance)
{
	FILE* tmpFp;
	char buf[62];
	int ret;

	tmpFp = fopen(TMP_FILE, "r+");
	if (tmpFp == NULL)
        fatal("opening Temporary File!");

	for(; lnum > 0; lnum--)
	{
		fgets(buf, sizeof(buf), fp);
		fputs(buf, tmpFp); // crash <----------------------------------------------------------------------
	}

	if (ptrCarInstance != NULL)
		fprintf(tmpFp, "%s %s %s %s\n", ptrCarInstance->id, ptrCarInstance->color, ptrCarInstance->manufact, ptrCarInstance->date);
	fgets(buf, sizeof(buf), fp);

	while(!feof(fp))
	{
		fgets(buf, sizeof(buf), fp);
		fputs(buf, tmpFp);
	}
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
