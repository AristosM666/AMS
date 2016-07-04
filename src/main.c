#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "misc.h"


const char ARCHIVE[] = "ams_archive.csv";


#define ID_COL '1'
#define COLOR_COL '2'
#define MANUFACT_COL '3'
#define DATE_COL '4'

const int MIN_DATE = 1960;
const int MAX_DATE = 2006;
const int MIN_ID = 1000;
const int MAX_ID = 9999;

typedef struct {
  char id[5];
  char color[30];
  char manufact[30];
  char date[5];
} Entry;


typedef struct {
  size_t entryNum;
  Entry entryInfo;
  enum { INVALID, REMOVE, WRITE } opType;
} Operation;


/* display error message and exit */
void fatal(char* errMsg);
void displaySplashScreen(void);
bool goToMainMenu(FILE* archiveFp, Operation* op);
void displayMainMenu(void);
void createNewEntry(FILE* archiveFp, Operation* op);
void removeEntry(FILE* archiveFp, Operation* op);
void modifyEntry(FILE* archiveFp, Operation* op);
void updateArchive(FILE* archiveFp, Operation* op);
bool goToSearchMenu(FILE* archiveFp);
void displaySearchMenu(void);
void displayTableHeader(void);
void displayTableFooter(void);
char getUserOption(void);
void clearScreen(void);
void strToUpper(char* str);
/* Initialize a Entry with empty strings and return it */
Entry createNullEntry(void);
void enterCarInfo(Entry* car);
Entry getEntryInfo(FILE* fp, size_t entryNum);
void displayEntryRow(Entry entry);
void displayAllEntrys(FILE* fp);
bool readNextEntry(FILE* fp, Entry* car);
/* Get entry count from archive */
size_t getEntryCount(FILE* fp);
/* Returns the entry number of the first matching entry */
size_t findNextMatchingEntry(FILE* fp, char* key, char col, char operand);
/* Overwrites/Removes archive entry */
void writeToArchive(FILE* fp, size_t entryNum, Entry* car);
/* Returns a string of all UNIQUE numeric values in the DATE or ID fields */
char* findAllNumeric(FILE* fp, char col);
/* Returns all UNIQUE alpha-numeric strings in the COLOR or MANUFACT fields */
char* findAllAlphaNumeric(FILE* fp, char col);


int main(void)
{
  FILE* archiveFp;
  Operation op;
  
  system("TITLE Automotive Managment System");

  clearScreen();
  displaySplashScreen();
  getchar();
  
  archiveFp = fopen(ARCHIVE, "r+");
  if (archiveFp == NULL)
    fatal("opening archive");
  
  while ( goToMainMenu(archiveFp, &op) );

  return EXIT_SUCCESS;
}


void displaySplashScreen(void)
{
  printf("\n\n\n\n\t\t[::] Automotive Management System [::]\n\n");
  printf("\t\tCollege Software Engineering Project.\n\n");
  printf("\t\tWritten By:\tAristos Miliaressis\n");
  printf("\t\t\tAt:\t10/10/2015\n");
  printf("\t\t\tIn:\tANSI-C\n\n");
  printf("\t\tPress [Enter] to Continue...");
  fflush(stdin);
}


bool goToMainMenu(FILE* archiveFp, Operation* op)
{
  char option;
  
  clearScreen();
  displayMainMenu();
  option = getUserOption();
    
  clearScreen();

  switch (option)
  {
  case '1':
    printf("\n\n\t\t[*] View Archive Entrys [*]\n\n");
    displayAllEntrys(archiveFp);

    printf("\n\n\t\tPress Any Key To Continue..");
    getchar();
    break;
  
  case '2':
    createNewEntry(archiveFp, op);
    break;
  
  case '3':
    printf("\n\n\t[*] Remove Car's Entry [*]\n\n");

    removeEntry(archiveFp, op);
    break;
  
  case '4':
    printf("\n\n\t[*] Modify Car's Entry [*]\n\n");
	
    modifyEntry(archiveFp, op);
    break;
  
  case '5':
    while ( goToSearchMenu(archiveFp) );
    break;
  
  case '6':
    printf("\n\n\tUpdating '%s' Archive..", ARCHIVE);

    updateArchive(archiveFp, op);
    break;
   
  case '0': 
    fclose(archiveFp);
    return false; 
  }
  
  return true;
}


void displayMainMenu(void)
{
  printf("\n\n\t[*] Main Menu [*]");
  printf("\n\n\t[1] - View All Entrys");
  printf("\n\t[2] - Create New Entry");
  printf("\n\t[3] - Remove Entry");
  printf("\n\t[4] - Modify Entry");
  printf("\n\t[5] - Search Archive");
  printf("\n\t[6] - Update Archive");
  printf("\n\t[0] - Exit");
  printf("\n\n\tSelect an Option (0-6): ");
  fflush(stdin);
}


void createNewEntry(FILE* archiveFp, Operation* op)
{
  enterCarInfo(&op->entryInfo);

  if (findNextMatchingEntry(archiveFp, op->entryInfo.id, ID_COL, 0) == 0)
  {
    op->entryNum = 0;
    op->opType = WRITE;
    printf("\n\n\tEntry Created Successfully!");
    getchar();
  }
  else
  {
    printf("\n\tError Entry with ID '%s' already Exists..", 
           op->entryInfo.id);
    getchar();
  }
}


void removeEntry(FILE* archiveFp, Operation* op)
{
  char *tmpPtr;
  
  tmpPtr = findAllNumeric(archiveFp, ID_COL);
  printf("\tEntrys (ID): %s", tmpPtr);
  free(tmpPtr);
  tmpPtr = NULL;

  printf("\n\n\tEnter car's ID number: ");
  getString(op->entryInfo.id, sizeof(op->entryInfo.id));

  op->entryNum = findNextMatchingEntry(archiveFp, 
                                        op->entryInfo.id, ID_COL, 0);
  if (op->entryNum == 0)
  {
    op->opType = INVALID;
    printf("\n\tEntry ID '%s' doesn't exist..", op->entryInfo.id);
  }
  else
  {
    op->opType = REMOVE;
    printf("\n\tEntry Removed Successfully!");
  }
  getchar();
}


void modifyEntry(FILE* archiveFp, Operation* op)
{
  char *tmpPtr;
  size_t i;
  
  tmpPtr = findAllNumeric(archiveFp, ID_COL);
  printf("\tEntrys (ID): %s", tmpPtr);
  free(tmpPtr);
  tmpPtr = NULL;
	
  printf("\n\n\tEnter car's ID number: ");
  getString(op->entryInfo.id, sizeof(op->entryInfo.id));

  op->entryNum = findNextMatchingEntry(archiveFp, op->entryInfo.id, ID_COL, 0);
  if (op->entryNum == 0)
  {
    op->opType = INVALID;
    printf("\n\tEntry ID '%s' doesn't exist..", op->entryInfo.id);
    getchar();
  }
  else
  {
    clearScreen();
    printf("\n\n\t\t[*] Enter New Entry's Info [*]\n\n");
    displayTableHeader();
    op->entryInfo = getEntryInfo(archiveFp, op->entryNum);
    displayEntryRow(op->entryInfo);
    displayTableFooter();
				
    i = atoi(op->entryInfo.id);
    enterCarInfo(&op->entryInfo);
    if (findNextMatchingEntry(archiveFp, op->entryInfo.id, ID_COL, 0) == 0 || 
                                          i == atoi(op->entryInfo.id))
    {
      op->opType = WRITE;
      printf("\n\n\tEntry Modified Successfully!");
      getchar();
    }
    else
    {
      op->opType = INVALID;
      printf("\n\tError Entry with ID '%s' already Exists..", op->entryInfo.id);
      getchar();
    }
  }
}


void updateArchive(FILE* archiveFp, Operation* op)
{
  if (op->opType == WRITE)
  {
    if (op->entryNum == 0) /* add entry */
    {
      fseek(archiveFp, 0, SEEK_END);
      fprintf(archiveFp, "%s,%s,%s,%s,", op->entryInfo.id, 
           op->entryInfo.color, op->entryInfo.manufact, op->entryInfo.date);
      fflush(archiveFp);
    }
    else /* modify entry */
    {
      writeToArchive(archiveFp, op->entryNum, &op->entryInfo);
    }
  }
  else if (op->opType == REMOVE)
  {
    writeToArchive(archiveFp, op->entryNum, NULL);
  }
  printf("\n\n\tUpdated Archive Successfully!");
  getchar();
		
  op->entryInfo = createNullEntry();
}


bool goToSearchMenu(FILE* archiveFp)
{
  char option, searchKey[32], *tmpPtr;
  size_t entryNum;
  Entry entry;
  
  clearScreen();
  displaySearchMenu();
  option = getUserOption();

  if (option == '0')
  {
    return false;
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
  {
    return true;
  }
  free(tmpPtr);
  tmpPtr = NULL;

  printf("\n\n\tEnter Search Term: ");
  getString(searchKey, sizeof(searchKey));

  /* Remove Line Feed Character */
  if (searchKey[strlen(searchKey)-1] == '\n') 
    searchKey[strlen(searchKey)-1] = '\0';
  strToUpper(&searchKey[0]);
			
  /* Test if provided value is valid */
  rewind(archiveFp);
  entryNum = findNextMatchingEntry(archiveFp, searchKey, 
                                        option, searchKey[0]);
  if (entryNum == 0)
  {
    printf("\n\tInvalid Search Key '%s'!", searchKey);
  }
  else /* Print Matching Entrys */
  {
    clearScreen();
    printf("\n\n\t[*] Search Results [*]\n");
    displayTableHeader();
    do 
    {
      entry = getEntryInfo(archiveFp, entryNum);
      displayEntryRow(entry);
      entryNum = findNextMatchingEntry(archiveFp, searchKey, 
                                            option, searchKey[0]);
    } while(entryNum != 0);
    displayTableFooter();
  }
  printf("\n\n\tPress Any Key To Continue..");
  getchar();
  return true;
}


void displaySearchMenu(void)
{
  printf("\n\n\t[*] Search Archive [*]");
  printf("\n\n\t[1] - Search By ID");
  printf("\n\t[2] - Search By Color");
  printf("\n\t[3] - Search By Manufacturer");
  printf("\n\t[4] - Search By Date");
  printf("\n\t[0] - Back to Main Menu");
  printf("\n\n\tSelect an Option (0-4): ");
  fflush(stdin);
}


void displayTableHeader(void)
{
  printf("\t _______________________________________________________________\n");
  printf("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
  printf("\t|---------------------------------------------------------------|");
}


void displayTableFooter(void)
{
  printf("\n\t|_______________________________________________________________|");
}


void fatal(char* errMsg)
{
  fprintf(stderr, "\n\n\t\tFailed while %s!", errMsg);
  perror("\n\t\tERROR");
  printf("\n\t\tPress [Enter] to Exit..");
  getchar();

  clearScreen();
  exit(EXIT_FAILURE);
}


void clearScreen(void)
{
  printf("\033[H\033[2J");
}


inline char getUserOption(void)
{
  char ch, *str = malloc(30);
  size_t i = 0;
  
  (void) getString(str, 30);
  do {
    ch = str[i];
    if (ch != ' ' && ch != '\n' && ch != '\t')
      return ch;
    i++;
  } while(ch != '\n');
  
  free(str);
  str = NULL;
  return ch;
}


void strToUpper(char* str)
{
  size_t i;
  for (i = 0; i < strlen(str); i++)
    str[i] = toupper(str[i]);
}


Entry createNullEntry(void)
{
  Entry car;

  car.id[0] = '\0';
  car.color[0] = '\0';
  car.manufact[0] = '\0';
  car.date[0] = '\0';

  return car;
}


void enterCarInfo(Entry* car)
{
  int i;

  do 
  {
    printf("\n\n\tEnter Entry's ID (%d..%d): ", MIN_ID, MAX_ID);
    getString(car->id, sizeof(car->id));

    if (isIntBetween(car->id, MIN_ID, MAX_ID))
      break;
		
    printf("\n\tInvalid ID Provided!");
    printf("\n\tAcceptable IDs are between %d and %d.", MIN_ID, MAX_ID);
  } while (true);
	
  printf("\n\n\tEnter Car's Color: ");
  getString(car->color, sizeof(car->color));
    
  printf("\n\n\tEnter Car's Manufacturer: ");
  getString(car->manufact, sizeof(car->manufact));
    
  /* Remove Leading Line Feed */
  if(car->color[strlen(car->color)-1] == '\n')
    car->color[strlen(car->color)-1] = '\0';
  strToUpper(&car->color[0]);

  /* Remove Leading Line Feed */
  if(car->manufact[strlen(car->manufact)-1] == '\n')
    car->manufact[strlen(car->manufact)-1] = '\0';
  strToUpper(&car->manufact[0]);

  do 
  {
    printf("\n\n\tEnter car's date of Manufacture (%d..%d): ", 
            MIN_DATE, MAX_DATE);
    getString(car->date, sizeof(car->date));

    if (isIntBetween(car->date, MIN_DATE, MAX_DATE))
      break;
			
    printf("\n\tInvalid Date Provided!");
    printf("\n\tAcceptable Dates are between %d and %d.", MIN_DATE, MAX_DATE);
  } while (true);
}


Entry getEntryInfo(FILE* fp, size_t entryNum)
{
  Entry entry = createNullEntry();
  int i;

  rewind(fp);
  for (i = 1; i <= entryNum; ++i)
    readNextEntry(fp, &entry);
  
  return entry;
}


void displayEntryRow(Entry entry)
{
  char tmp[30];
  
  strcpy(tmp, entry.manufact);
  if (strlen(tmp) < 8)
    strcat(tmp, "\t");
	
  printf("\n\t| %s\t\t%s\t\t%s\t\t%s\t|", 
         entry.id, entry.color, tmp, entry.date);
}


void displayAllEntrys(FILE* fp)
{
  Entry entry;
  size_t i;
	
  rewind(fp);
  displayTableHeader();
  for (i = 0; i < getEntryCount(fp); i++)
  {
    entry = getEntryInfo(fp, i);
    displayEntryRow(entry);
  }
  displayTableFooter();
}


bool readNextEntry(FILE* fp, Entry* car)
{
  if (!csvReadNextVal(fp, &car->id[0]))
    return false;
  else if (!csvReadNextVal(fp, &car->color[0]))
    return false;
  else if (!csvReadNextVal(fp, &car->manufact[0]))
    return false;
  else if (!csvReadNextVal(fp, &car->date[0]))
    return false;
  else
    return true;
}


size_t findNextMatchingEntry(FILE* fp, char* key, char col, char operand)
{
  static size_t entryCount = 0;
  Entry car;
	
  strToUpper(key);
  if ((col == ID_COL || col == DATE_COL) && operand != '\0')
    key++;

  while(!feof(fp))
  {
    entryCount++;
    readNextEntry(fp, &car);

    switch (col)
    {
      case ID_COL: 
      {
        if ((operand == '>' && atoi(car.id) > atoi(key)) ||
            (operand == '<' && atoi(car.id) < atoi(key)) ||
            (operand == '\0' && !strcmp(car.id, key)))
          return entryCount;
        break;
      } 
      case COLOR_COL: 
      {
        if (strncmp(key, car.color, strlen(key)) == 0) 
          return entryCount;
        break;
      } 
      case MANUFACT_COL: 
      {
        if (strncmp(key, car.manufact, strlen(key)) == 0) 
          return entryCount;
        break;
      }
      case DATE_COL: 
      {
        if ((operand == '>' && atoi(car.date) > atoi(key)) ||
            (operand == '<' && atoi(car.date) < atoi(key)))
          return entryCount;
      }
    }
  }
	
  entryCount = 0;
  return entryCount;
}


char* findAllNumeric(FILE* fp, char col)
{
  const size_t entryCount = getEntryCount(fp);
  char* buf = (char*) malloc((entryCount * 6) + 1);
  int* entrys = (int*) malloc(entryCount * sizeof(int));
  char tmpBuf[10];
  size_t i;
  Entry car;
	
  if (buf == NULL)
    fatal("allocating memory");

  rewind(fp);
  for (i = 0; i < entryCount; i++)
  {
    readNextEntry(fp, &car);
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
  entrys = NULL;

  return buf;
}


char* findAllAlphaNumeric(FILE* fp, char col)
{
  const size_t entryCount = getEntryCount(fp);
  char** arr = (char**) malloc(entryCount * sizeof(char*));
  char* buf = (char*) malloc(entryCount * 32);
  size_t i;
  Entry car;

  if (arr == NULL || buf == NULL)
    fatal("allocating memory");
	
  rewind(fp);
  for (i = 0; i < entryCount; i++)
  {
    arr[i] = (char *) malloc(sizeof(char) * 30);
    if (arr[i] == NULL)
      fatal("allocating memory");
		
    readNextEntry(fp, &car);
    col == COLOR_COL ? strcpy(arr[i], car.color) : strcpy(arr[i], car.manufact);
  }
  i = removeDuplicate(&arr[0], entryCount);
	
  strcpy(buf, arr[0]);
  free(arr[0]);
  arr[0] = NULL;
  for (; i > 0; i--)
  {
    strcat(buf, ", ");
    strcat(buf, arr[i]);
		
    free(arr[i]);
    arr[i] = NULL;
  }
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


void writeToArchive(FILE* fp, size_t entryNum, Entry* car)
{
  Entry tmpCar = createNullEntry();
  FILE* tmpFp;
  const char tmpFileName[] = "fRpZX6EPZV";
  int ret;

  tmpFp = fopen(tmpFileName, "w");
  if (tmpFp == NULL)
    fatal("opening temporary file");

  rewind(fp);
  for(; entryNum > 1; --entryNum)
  {
    readNextEntry(fp, &tmpCar);
    fprintf(tmpFp, "%s,%s,%s,%s,", tmpCar.id, tmpCar.color, 
            tmpCar.manufact, tmpCar.date);
    fflush(tmpFp);
  }
	
  if (car != NULL)
  {
    fprintf(tmpFp, "%s,%s,%s,%s,", car->id, car->color, 
            car->manufact, car->date);
    fflush(tmpFp);
  }
  readNextEntry(fp, &tmpCar);

  while(readNextEntry(fp, &tmpCar))
  {
    fprintf(tmpFp, "%s,%s,%s,%s,", tmpCar.id, tmpCar.color, 
            tmpCar.manufact, tmpCar.date);
    fflush(tmpFp);
  }
  fclose(tmpFp);
  fclose(fp);

  ret = remove(ARCHIVE);
  if (ret == -1)
    fatal("removing outdated archive");

  ret = rename(tmpFileName, ARCHIVE);
  if (ret == -1)
    fatal("renaming archive");

  fp = fopen(ARCHIVE, "r+");
  if (fp == NULL)
    fatal("reopening archive");
}
