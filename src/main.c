#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include "misc.h"


const char ARCHIVE[] = "ams_archive.csv";


#define ID_COL '1'
#define COLOR_COL '2'
#define MANUFACT_COL '3'
#define DATE_COL '4'

const short int MIN_DATE = 1960;
const short int MAX_DATE = 2006;
const short int MIN_ID = 1000;
const short int MAX_ID = 9999;

struct Entry 
{
  char id[5];
  char color[30];
  char manufact[30];
  char date[5];
};
typedef struct Entry entry_t;

#define MAX_ENTRYS 512
entry_t *entryTable[MAX_ENTRYS];
ssize_t entryCount = 0;


struct Operation
{
  enum { INVALID=0, REMOVE, OVERWRITE, APPEND } opType;
  char oldId[5];
  entry_t *entryInfo;
};
typedef struct Operation operation_t;


void displaySplashScreen (void);
void displayMainMenu (void);
bool goToMainMenu (FILE **archiveFp, operation_t *const op);
void createNewEntry (operation_t *const op);
void removeEntry (operation_t *const op);
void modifyEntry (operation_t *const op);
void updateArchive (FILE **archiveFp, operation_t *const op);
void displayEntryTable (entry_t *entryTable[]);
void displayTableHeader (void);
void displayTableFooter (void);
void displayEntryRow (const entry_t *const entry);
void displaySearchMenu (void);
bool goToSearchMenu (void);
char *getSearchTerm (void);
char getUserOption (void);
void printAllColors (void); 
void printAllManufacts (void); 
void printAllIDs (void); 
void printAllDates (void);
void enterCarInfo (entry_t *const entry);
bool readEntry (FILE *archiveFp, entry_t *const entry);
void loadArchive (FILE *archiveFp);
bool lookupEntryByID (const char *const id, entry_t *const entry);
void copyEntry (entry_t *const dest, const entry_t *const src);
entry_t **findAllMatching (char *key, const char col, const char operand);
size_t getIndexFromID (const char *const entryId);
void writeToArchive (FILE **archiveFp, const char *const entryId, const entry_t *const entry);


int 
main (void)
{
    entry_t entry;
    FILE *archiveFp;
    operation_t op = { 
        .opType = INVALID,
        .entryInfo = malloc (sizeof (entry_t)) 
    };
        
    system ("TITLE Automotive Managment System");

    clearScreen ();
    displaySplashScreen ();
    getchar ();
  
    archiveFp = fopen (ARCHIVE, "a+");
    if (archiveFp == NULL)
        fatal ("opening archive");
    loadArchive (archiveFp);

    while ( goToMainMenu (&archiveFp, &op) );

    for (; entryCount >= 0; entryCount--)
        free (entryTable[entryCount]);
    free (op.entryInfo);
    fclose (archiveFp);
    
    return EXIT_SUCCESS;
}


void 
displaySplashScreen (void)
{
    printf ("\n\n\n\n\t\t[::] Automotive Management System [::]\n\n");
    printf ("\t\tCollege Software Engineering Project.\n\n");
    printf ("\t\tWritten By:\tAristos Miliaressis\n");
    printf ("\t\t\tAt:\t10/10/2015\n");
    printf ("\t\t\tIn:\tANSI-C\n\n");
    printf ("\t\tPress [Enter] to continue...");
    fflush (stdin);
}


void 
loadArchive (FILE *archiveFp)
{
    entry_t *eptr = malloc (sizeof (entry_t));

    while (readEntry (archiveFp, eptr))
      {
        entryTable[entryCount] = eptr;
        entryCount++;
        eptr = malloc (sizeof (entry_t));
      }

    free (eptr);
    entryTable[entryCount] = NULL;
}


bool 
goToMainMenu (FILE **archiveFp, operation_t *const op)
{
    char option;
  
    clearScreen ();
    displayMainMenu ();
    option = getUserOption ();
    
    clearScreen ();
    switch (option)
      {
      case '1':
        printf ("\n\n\t[*] Display All Entrys [*]\n");
        displayEntryTable (entryTable);

        printf ("\n\n\tPress [Enter] to continue...");
        getchar ();
        break;
  
      case '2':
        createNewEntry (op);
        break;
  
      case '3':
        printf ("\n\n\t[*] Remove Entry [*]\n\n");

        removeEntry (op);
        break;
  
      case '4':
        printf ("\n\n\t[*] Modify Entry [*]\n\n");
	
        modifyEntry (op);
        break;
  
      case '5':
        while ( goToSearchMenu () );
        break;
  
      case '6':
        printf ("\n\n\tUpdating '%s' archive..", ARCHIVE);

        updateArchive (archiveFp, op);
        break;
   
      case '0':
        return false; 
    }

  return true;
}


void 
displayMainMenu (void)
{
    printf ("\n\n\t[*] Main Menu [*]");
    printf ("\n\n\t[1] - View All Entrys");
    printf ("\n\t[2] - Create New Entry");
    printf ("\n\t[3] - Remove Entry");
    printf ("\n\t[4] - Modify Entry");
    printf ("\n\t[5] - Search Archive");
    printf ("\n\t[6] - Update Archive");
    printf ("\n\t[0] - Exit");
    printf ("\n\n\tSelect an Option (0-6): ");
    fflush (stdin);
}


void 
printAllIDs (void)
{
    int i, idList[entryCount];
    
    if (entryCount == 0)
        return;
    
    for (i = 0; i < entryCount; i++)
        idList[i] = atoi (entryTable[i]->id);
    
    quicksort (idList, 0, i-1);
    
    printf ("\n\tIDs: %d", idList[0]);
    for (i = 1; i < entryCount; i++)
        printf (", %d", idList[i]);
}


void 
printAllColors (void)
{
    char **colorList = (char**) malloc ((entryCount + 1) * sizeof (char*));
    int i;
    
    if (entryCount == 0)
        return;
    
    for (i = 0; i <= entryCount; i++)
        colorList[i] = entryTable[i]->color;
    i--;
  
    i = (int) removeDuplicateStr (colorList, i);
    
    printf ("\n\tColors: %s", colorList[0]);
    for (; i > 0; --i)
        printf (", %s", colorList[i]);
  
    free (colorList);
    colorList = NULL;
}


void 
printAllManufacts (void)
{
    char **manufactList = (char**) malloc ((entryCount + 1) * sizeof (char*));
    int i;
    
    if (entryCount == 0)
        return;
    
    for (i = 0; i <= entryCount; i++)
        manufactList[i] = entryTable[i]->manufact;
    i--;
  
    i = (int) removeDuplicateStr (manufactList, i);
    
    printf ("\n\tManufacturer: %s", manufactList[0]);
    for (; i > 0; --i)
        printf (", %s", manufactList[i]);
    
    free (manufactList);
    manufactList = NULL;
}


void 
printAllDates (void)
{
    int i, j, *dateList = (int*) malloc ((entryCount + 1) * sizeof (int));
    
    if (entryCount == 0)
        return;
    
    for (i = 0; i < entryCount; i++)
        dateList[i] = atoi (entryTable[i]->date);
    i--;
  
    i = (int) removeDuplicate (dateList, i);
    
    quicksort (dateList, 0, i);
    
    printf ("\n\tManufact Dates: %d", dateList[0]);
    for (j = 1; j <= i; j++)
        printf (", %d", dateList[j]);
  
    free (dateList);
    dateList = NULL;
}


void 
createNewEntry (operation_t *const op)
{
    enterCarInfo (op->entryInfo);

    if (!lookupEntryByID (op->entryInfo->id, NULL))
      {
        op->opType = APPEND;
        printf ("\n\n\tEntry created successfully!");
        getchar ();
      }
    else
      {
        op->opType = INVALID;
        printf ("\n\tError ID '%s' already in use..", op->entryInfo->id);
        getchar ();
      }
}


void 
removeEntry (operation_t *const op)
{
    printAllIDs ();

    printf ("\n\n\tEnter car's ID number: ");
    getString (op->entryInfo->id, sizeof (op->entryInfo->id));

    if (!lookupEntryByID (op->entryInfo->id, op->entryInfo))
      {
        op->opType = INVALID;
        printf ("\n\tEntry ID '%s' doesn't exist..", op->entryInfo->id);
      }
    else
      {
        op->opType = REMOVE;
        printf ("\n\tEntry removed successfully!");
      }
    getchar ();
}


void 
modifyEntry (operation_t *const op)
{
    printAllIDs ();
	
    printf ("\n\n\tEnter car's ID number: ");
    getString (op->entryInfo->id, sizeof (op->entryInfo->id));

    if (!lookupEntryByID (op->entryInfo->id, op->entryInfo))
      {
        op->opType = INVALID;
        printf ("\n\tEntry ID '%s' doesn't exist..", op->entryInfo->id);
      }
    else
      {
        clearScreen ();
        printf ("\n\n\t\t[*] Enter New Info [*]\n\n");
        displayTableHeader ();
        displayEntryRow (op->entryInfo);
        displayTableFooter ();
        
        int i = atoi (op->entryInfo->id);
        strcpy (op->oldId, op->entryInfo->id);
				
        enterCarInfo (op->entryInfo);
        if (!lookupEntryByID (op->entryInfo->id, NULL) || i == atoi (op->entryInfo->id))
          {
            op->opType = OVERWRITE;
            printf ("\n\n\tEntry modified successfully!");
          }
        else
          {
            op->opType = INVALID;
            printf ("\n\tError ID '%s' already in use..", op->entryInfo->id);
          }
      }
    getchar ();
}


void 
updateArchive (FILE **archiveFp, operation_t *const op)
{
    size_t index;
    
    if (op->opType == APPEND)
      {
        writeToArchive (archiveFp, NULL, op->entryInfo);
        entryTable[entryCount] = malloc (sizeof (entry_t));
        copyEntry (entryTable[entryCount], op->entryInfo);
        entryCount++;
      }
    else if (op->opType == OVERWRITE)
      {
        writeToArchive (archiveFp, op->oldId, op->entryInfo);
        index = getIndexFromID (op->oldId);
        copyEntry (entryTable[index], op->entryInfo);
      }
    else if (op->opType == REMOVE)
      {
        writeToArchive (archiveFp, op->entryInfo->id, NULL);
        index = getIndexFromID (op->entryInfo->id);
        free (entryTable[index]);
        for (; index <= entryCount; index++)
            entryTable[index] = entryTable[index+1];
        entryCount--;
      }

    printf ("\n\n\tUpdated archive successfully!");
    getchar ();
}


bool 
goToSearchMenu (void)
{
    entry_t **resultTable;
    char option, *searchKey;
  
    clearScreen ();
    displaySearchMenu ();
    option = getUserOption ();

    switch (option)
      {
      case '0':
          return false;
      case '1':
          printAllIDs ();
          break;
      case '2':
          printAllColors ();
          break;
      case '3':
          printAllManufacts ();
          break;
      case '4':
          printAllDates ();
          break;
      default:
          return true;
      }
  
    searchKey = getSearchTerm ();
    resultTable = findAllMatching (searchKey, option, searchKey[0]);    
    
    clearScreen ();
    printf ("\n\n\t[*] Search Results [*]\n");
    displayEntryTable (resultTable);

    free (resultTable);
    free (searchKey);
    resultTable = NULL;  
    searchKey = NULL;
  
    printf ("\n\n\tPress [Enter] to continue...");
    getchar ();
    return true;
}


char *
getSearchTerm (void)
{
    char *searchKey = malloc (32U);
  
    printf ("\n\n\tEnter search term: ");
    getString (searchKey, 32U);
  
    parseWhiteSpace (searchKey);
    strToUpper (searchKey);
  
    return searchKey;
}


void 
displaySearchMenu (void)
{
    printf ("\n\n\t[*] Search Archive [*]");
    printf ("\n\n\t[1] - Search By ID");
    printf ("\n\t[2] - Search By Color");
    printf ("\n\t[3] - Search By Manufacturer");
    printf ("\n\t[4] - Search By Date");
    printf ("\n\t[0] - Back to Main Menu");
    printf ("\n\n\tSelect an Option (0-4): ");
    fflush (stdin);
}


void 
displayTableHeader (void)
{
    printf ("\t _______________________________________________________________\n");
    printf ("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
    printf ("\t|---------------------------------------------------------------|");
}


void 
displayTableFooter (void)
{
    printf ("\n\t|_______________________________________________________________|");
}


char 
getUserOption (void)
{
    char ch, *str = malloc (30U);
  
    (void) getString (str, 30U);
    parseWhiteSpace (str);
  
    ch = str[0];
    free (str);
    str = NULL;
  
    return ch;
}


void 
enterCarInfo (entry_t *const entry)
{
    do 
      {
        printf ("\n\n\t Entry's ID (%d..%d): ", MIN_ID, MAX_ID);
        getString (entry->id, sizeof (entry->id));

        if (isIntBetween (entry->id, MIN_ID, MAX_ID))
            break;
		
        printf ("\n\tInvalid ID provided!");
        printf ("\n\tAcceptable IDs are between %d and %d.", 
                MIN_ID, MAX_ID);
      } 
    while (true);
	
    printf ("\n\n\t Car's Color: ");
    getString (entry->color, sizeof (entry->color));
    strToUpper (entry->color);
    
    printf ("\n\n\t Car's Manufacturer: ");
    getString (entry->manufact, sizeof (entry->manufact));  
    strToUpper (entry->manufact);

    do 
      {
        printf ("\n\n\tCar's date of manufacture (%d..%d): ", 
                MIN_DATE, MAX_DATE);
        getString (entry->date, sizeof (entry->date));

        if (isIntBetween (entry->date, MIN_DATE, MAX_DATE))
            break;
			
        printf ("\n\tInvalid date provided!");
        printf ("\n\tAcceptable dates are between %d and %d.", 
                MIN_DATE, MAX_DATE);
      } 
    while (true);
}


void
displayEntryRow (const entry_t *const entry)
{
    char tmp[30] = { 0 };
  
    strcpy (tmp, entry->manufact);
    if (strlen (tmp) < 8)
        strcat (tmp, "\t");
	
    printf ("\n\t| %s\t\t%s\t\t%s\t\t%s\t|", 
            entry->id, entry->color, tmp, entry->date);
}


void 
displayEntryTable (entry_t *entryTable[])
{
    size_t i = 0;
    
    displayTableHeader ();
    while (entryTable[i] != NULL)
      {
        displayEntryRow (entryTable[i]);
        i++;
      }
  
    if (i == 0)
        printf ("\n\t|                       No Entrys Found!                        |");
    
    displayTableFooter ();
}


bool 
readEntry (FILE *archiveFp, entry_t *const entry)
{
    return !(  !csvReadNextVal (archiveFp, entry->id) 
            || !csvReadNextVal (archiveFp, entry->color) 
            || !csvReadNextVal (archiveFp, entry->manufact)
            || !csvReadNextVal (archiveFp, entry->date)
           );
}


bool 
lookupEntryByID (const char *const id, entry_t *const eptr)
{
    size_t i;
    
    for (i = 0; i < entryCount; i++)
      {
        if (atoi (entryTable[i]->id) == atoi (id))
          {
            if (eptr != NULL)
                copyEntry (eptr, entryTable[i]);
            return true;
          }
      }
	
    return false;
}


void 
copyEntry (entry_t *const dest, const entry_t *const src)
{
    strcpy (dest->id, src->id);
    strcpy (dest->color, src->color);
    strcpy (dest->manufact, src->manufact);
    strcpy (dest->date, src->date);
}


entry_t ** 
findAllMatching (char *key, const char col, const char operand)
{
    entry_t **resultTable = malloc (entryCount * sizeof (entry_t*));
    size_t i, resultCount = 0;
	
    strToUpper (key);
    if ( (col == ID_COL || col == DATE_COL) && operand != '\0' )
        key++;

    for (i = 0; i < entryCount; i++)
      {
        switch (col)
          {
          case ID_COL: 
            {
              if ((operand == '>'  && atoi (entryTable[i]->id) >  atoi (key)) ||
                  (operand == '<'  && atoi (entryTable[i]->id) <  atoi (key)) ||
                  (operand == '\0' && atoi (entryTable[i]->id) == atoi (key)))
                {
                  resultTable[resultCount] = entryTable[i];
                  resultCount++;
                }
              break;
            } 
          case COLOR_COL: 
            {
              if (strncmp (key, entryTable[i]->color, strlen (key)) == 0)
                {
                  resultTable[resultCount] = entryTable[i];
                  resultCount++;
                }
              break;
            } 
          case MANUFACT_COL: 
            {
              if (strncmp (key, entryTable[i]->manufact, strlen (key)) == 0)
                {
                  resultTable[resultCount] = entryTable[i];
                  resultCount++;
                }
              break;
            }
          case DATE_COL: 
            {
              if ((operand == '>' && atoi (entryTable[i]->date) > atoi (key)) || 
                  (operand == '<' && atoi (entryTable[i]->date) < atoi (key)))
                {
                  resultTable[resultCount] = entryTable[i];
                  resultCount++;
                }
            }
          }
      }

    return resultTable;
}


size_t 
getIndexFromID (const char *const entryId)
{
  size_t index;
  for (index = 0; index < entryCount; index++)
    {
      if (!strcmp (entryId, entryTable[index]->id))
          return index;
    }
}


void
writeToArchive (FILE **archiveFp, 
                const char *const entryId, 
                const entry_t *const entry)
{
    static entry_t tmpEntry;
    const char tmpFileName[] = "fRpZX6EPZV";
    FILE *tmpFp;
    ssize_t ret;
    
    if (entryId == NULL) /* Append Entry */
      {
        fseek (*archiveFp, -1, SEEK_END);
        fprintf (*archiveFp, "%s,%s,%s,%s,", 
                entry->id, 
                entry->color, 
                entry->manufact, 
                entry->date);
        fflush (*archiveFp);
        return;
      }

    tmpFp = fopen (tmpFileName, "w");
    if (tmpFp == NULL)
        fatal ("opening temporary file");

    rewind (*archiveFp);
    while (readEntry (*archiveFp, &tmpEntry))
      {
        if (!strcmp (entryId, tmpEntry.id))
            break;
        
        fprintf (tmpFp, "%s,%s,%s,%s,", 
                 tmpEntry.id, 
                 tmpEntry.color, 
                 tmpEntry.manufact, 
                 tmpEntry.date);
        fflush (tmpFp);
      }
    	
    if (entry != NULL) /* Overwrite Entry */
      {
        fprintf (tmpFp, "%s,%s,%s,%s,", 
                 entry->id, 
                 entry->color, 
                 entry->manufact, 
                 entry->date);
        fflush (tmpFp);
      }

    while (readEntry (*archiveFp, &tmpEntry))
      {
        fprintf (tmpFp, "%s,%s,%s,%s,", 
                 tmpEntry.id, 
                 tmpEntry.color, 
                 tmpEntry.manufact, 
                 tmpEntry.date);
        fflush (tmpFp);
      }
    fclose (*archiveFp);
    fclose (tmpFp);

    ret = remove (ARCHIVE);
    if (ret == -1)
        fatal ("removing outdated archive");

    ret = rename (tmpFileName, ARCHIVE);
    if (ret == -1)
        fatal ("renaming archive");

    *archiveFp = fopen (ARCHIVE, "a+");
    if (*archiveFp == NULL)
        fatal ("reopening archive");
}

