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

struct Entry {
  char id[5];
  char color[30];
  char manufact[30];
  char date[5];
};
typedef struct Entry entry_t;


struct Operation {
  size_t entryNum;
  entry_t entryInfo;
  enum { INVALID, REMOVE, WRITE } opType;
};
typedef struct Operation operation_t;


void displaySplashScreen(void);
bool goToMainMenu(FILE* archiveFp, operation_t* op);
void displayMainMenu(void);
void createNewEntry(FILE* archiveFp, operation_t* op);
void removeEntry(FILE* archiveFp, operation_t* op);
void modifyEntry(FILE* archiveFp, operation_t* op);
void updateArchive(FILE* archiveFp, operation_t* op);
bool goToSearchMenu(FILE* archiveFp);
void displaySearchMenu(void);
void displayTableHeader(void);
void displayTableFooter(void);
char getUserOption(void);
/* Initialize a Entry with empty strings and return it */
entry_t createNullEntry(void);
void enterCarInfo(entry_t* entry);
entry_t getEntryInfo(FILE* fp, size_t entryNum);
void displayEntryRow(entry_t entry);
void displayAllEntrys(FILE* fp);
bool readNextEntry(FILE* fp, entry_t* entry);
/* Get entry count from archive */
size_t getEntryCount(FILE* fp);
/* Returns the entry number of the first matching entry */
size_t findNextMatchingEntry(FILE* fp, char* key, char col, char operand);
/* Overwrites/Removes archive entry */
void writeToArchive(FILE* fp, size_t entryNum, entry_t* entry);
/* Returns a string of all UNIQUE numeric values in the DATE or ID fields */
char* findAllNumeric(FILE* fp, char col);
/* Returns all UNIQUE alpha-numeric strings in the COLOR or MANUFACT fields */
char* findAllAlphaNumeric(FILE* fp, char col);


int main(void)
{
  FILE* archiveFp;
  operation_t op;
  
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
  printf("\t\tPress [Enter] to continue...");
  fflush(stdin);
}


bool goToMainMenu(FILE* archiveFp, operation_t* op)
{
  char option;
  
  clearScreen();
  displayMainMenu();
  option = getUserOption();
    
  clearScreen();

  switch (option)
  {
  case '1':
    printf("\n\n\t[*] Display All Entrys [*]\n");
    displayAllEntrys(archiveFp);

    printf("\n\n\tPress [Enter] to continue...");
    getchar();
    break;
  
  case '2':
    createNewEntry(archiveFp, op);
    break;
  
  case '3':
    printf("\n\n\t[*] Remove Entry [*]\n\n");

    removeEntry(archiveFp, op);
    break;
  
  case '4':
    printf("\n\n\t[*] Modify Entry [*]\n\n");
	
    modifyEntry(archiveFp, op);
    break;
  
  case '5':
    while ( goToSearchMenu(archiveFp) );
    break;
  
  case '6':
    printf("\n\n\tUpdating '%s' archive..", ARCHIVE);

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


void createNewEntry(FILE* archiveFp, operation_t* op)
{
  enterCarInfo(&op->entryInfo);

  if (findNextMatchingEntry(archiveFp, op->entryInfo.id, ID_COL, 0) == 0)
  {
    op->entryNum = 0;
    op->opType = WRITE;
    printf("\n\n\tEntry created successfully!");
    getchar();
  }
  else
  {
    printf("\n\tError ID '%s' already in use..", 
           op->entryInfo.id);
    getchar();
  }
}


void removeEntry(FILE* archiveFp, operation_t* op)
{
  char *tmpPtr;
  
  tmpPtr = findAllNumeric(archiveFp, ID_COL);
  printf("\tIDs: %s", tmpPtr);
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
    printf("\n\tEntry removed successfully!");
  }
  getchar();
}


void modifyEntry(FILE* archiveFp, operation_t* op)
{
  char *tmpPtr;
  size_t i;
  
  tmpPtr = findAllNumeric(archiveFp, ID_COL);
  printf("\tIDs: %s", tmpPtr);
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
    printf("\n\n\t\t[*] Enter New Info [*]\n\n");
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
      printf("\n\n\tEntry modified successfully!");
      getchar();
    }
    else
    {
      op->opType = INVALID;
      printf("\n\tError ID '%s' already in use..", op->entryInfo.id);
      getchar();
    }
  }
}


void updateArchive(FILE* archiveFp, operation_t* op)
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
  printf("\n\n\tUpdated archive successfully!");
  getchar();
		
  op->entryInfo = createNullEntry();
}


bool goToSearchMenu(FILE* archiveFp)
{
  char option, searchKey[32], *tmpPtr;
  size_t entryNum;
  entry_t entry;
  
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
    printf("\n\tIDs: %s", tmpPtr);
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

  printf("\n\n\tEnter search term: ");
  getString(searchKey, sizeof(searchKey));

  /* Remove Line Feed Character */
  if (searchKey[strlen(searchKey)-1] == '\n') 
    searchKey[strlen(searchKey)-1] = '\0';
  
  parseWhiteSpace(&searchKey[0]);
  strToUpper(&searchKey[0]);
  
			
  /* Test if provided value is valid */
  rewind(archiveFp);
  entryNum = findNextMatchingEntry(archiveFp, searchKey, 
                                        option, searchKey[0]);
  if (entryNum == 0)
  {
    printf("\n\tInvalid search term '%s'!", searchKey);
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
  printf("\n\n\tPress [Enter] to continue...");
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
  } while(ch != '\n'); // replace with parseWhiteSpace();
  //ch = str[0];
  
  free(str);
  str = NULL;
  return ch;
}


entry_t createNullEntry(void)
{
  entry_t entry;

  entry.id[0] = '\0';
  entry.color[0] = '\0';
  entry.manufact[0] = '\0';
  entry.date[0] = '\0';

  return entry;
}


void enterCarInfo(entry_t* entry)
{
  int i;

  do 
  {
    printf("\n\n\t Entry's ID (%d..%d): ", MIN_ID, MAX_ID);
    getString(entry->id, sizeof(entry->id));

    if (isIntBetween(entry->id, MIN_ID, MAX_ID))
      break;
		
    printf("\n\tInvalid ID provided!");
    printf("\n\tAcceptable IDs are between %d and %d.", MIN_ID, MAX_ID);
  } while (true);
	
  printf("\n\n\t Car's Color: ");
  getString(entry->color, sizeof(entry->color));
    
  printf("\n\n\t Car's Manufacturer: ");
  getString(entry->manufact, sizeof(entry->manufact));
    
  /* Remove Leading Line Feed */
  if(entry->color[strlen(entry->color)-1] == '\n')
    entry->color[strlen(entry->color)-1] = '\0';
  strToUpper(&entry->color[0]);

  /* Remove Leading Line Feed */
  if(entry->manufact[strlen(entry->manufact)-1] == '\n')
    entry->manufact[strlen(entry->manufact)-1] = '\0';
  strToUpper(&entry->manufact[0]);

  do 
  {
    printf("\n\n\tCar's date of manufacture (%d..%d): ", 
            MIN_DATE, MAX_DATE);
    getString(entry->date, sizeof(entry->date));

    if (isIntBetween(entry->date, MIN_DATE, MAX_DATE))
      break;
			
    printf("\n\tInvalid date provided!");
    printf("\n\tAcceptable dates are between %d and %d.", MIN_DATE, MAX_DATE);
  } while (true);
}


entry_t getEntryInfo(FILE* fp, size_t entryNum)
{
  entry_t entry = createNullEntry();
  int i;

  rewind(fp);
  for (i = 1; i <= entryNum; ++i)
    readNextEntry(fp, &entry);
  
  return entry;
}


void displayEntryRow(entry_t entry)
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
  entry_t entry;
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


bool readNextEntry(FILE* fp, entry_t* entry)
{
  if (!csvReadNextVal(fp, &entry->id[0]))
    return false;
  else if (!csvReadNextVal(fp, &entry->color[0]))
    return false;
  else if (!csvReadNextVal(fp, &entry->manufact[0]))
    return false;
  else if (!csvReadNextVal(fp, &entry->date[0]))
    return false;
  else
    return true;
}


size_t findNextMatchingEntry(FILE* fp, char* key, char col, char operand)
{
  static size_t entryCount = 0;
  entry_t entry;
	
  strToUpper(key);
  if ((col == ID_COL || col == DATE_COL) && operand != '\0')
    key++;

  while(!feof(fp))
  {
    entryCount++;
    readNextEntry(fp, &entry);

    switch (col)
    {
      case ID_COL: 
      {
        if ((operand == '>' && atoi(entry.id) > atoi(key)) ||
            (operand == '<' && atoi(entry.id) < atoi(key)) ||
            (operand == '\0' && !strcmp(entry.id, key)))
          return entryCount;
        break;
      } 
      case COLOR_COL: 
      {
        if (strncmp(key, entry.color, strlen(key)) == 0) 
          return entryCount;
        break;
      } 
      case MANUFACT_COL: 
      {
        if (strncmp(key, entry.manufact, strlen(key)) == 0) 
          return entryCount;
        break;
      }
      case DATE_COL: 
      {
        if ((operand == '>' && atoi(entry.date) > atoi(key)) ||
            (operand == '<' && atoi(entry.date) < atoi(key)))
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
  entry_t entry;
	
  if (buf == NULL)
    fatal("allocating memory");

  rewind(fp);
  for (i = 0; i < entryCount; i++)
  {
    readNextEntry(fp, &entry);
    entrys[i] = (col == ID_COL) ? atoi(entry.id) : atoi(entry.date);
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
  entry_t entry;

  if (arr == NULL || buf == NULL)
    fatal("allocating memory");
	
  rewind(fp);
  for (i = 0; i < entryCount; i++)
  {
    arr[i] = (char *) malloc(sizeof(char) * 30);
    if (arr[i] == NULL)
      fatal("allocating memory");
		
    readNextEntry(fp, &entry);
    col == COLOR_COL ? strcpy(arr[i], entry.color) : strcpy(arr[i], entry.manufact);
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


void writeToArchive(FILE* fp, size_t entryNum, entry_t* entry)
{
  entry_t tmpCar = createNullEntry();
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
	
  if (entry != NULL)
  {
    fprintf(tmpFp, "%s,%s,%s,%s,", entry->id, entry->color, 
            entry->manufact, entry->date);
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
