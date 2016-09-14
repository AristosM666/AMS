#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/types.h>
#include "misc.h"


const char ARCHIVE[] = "ams_archive.csv";


#define ID_COL '1'
#define COLOR_COL '2'
#define MANUFACT_COL '3'
#define DATE_COL '4'

const uint16_t MIN_DATE = 1960;
const uint16_t MAX_DATE = 2006;
const uint16_t MIN_ID = 1000;
const uint16_t MAX_ID = 9999;

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
size_t entryCount = 0;


struct Operation
{
  enum { INVALID=0, REMOVE, OVERWRITE, APPEND } opType;
  entry_t *entryInfo;
  char oldId[5];
};
typedef struct Operation operation_t;


void displaySplashScreen (void);
void displayMainMenu (void);
bool goToMainMenu (FILE **archiveFp, operation_t *const op);
void createNewEntry (operation_t *const op);
void removeEntry (operation_t *const op);
void modifyEntry (operation_t *const op);
void updateArchive (FILE **archiveFp, operation_t *const op);
void pause (void);
void displayEntryTable (entry_t *table[]);
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
intmax_t getIndexFromID (const char *const entryId);
void writeToArchive (FILE **archiveFp, const char *const entryId, const entry_t *const entry);


int
main (void)
{
    operation_t op = {0};
    
    op.entryInfo = (entry_t*) calloc (1U, sizeof (entry_t));
    if (op.entryInfo == NULL)
        fatal ("allocating memory");

    clearScreen ();
    displaySplashScreen ();

    FILE *archiveFp = fopen (ARCHIVE, "a+");
    if (archiveFp == NULL)
        fatal ("opening archive");
    loadArchive (archiveFp);

    while ( goToMainMenu (&archiveFp, &op) );

    while (entryCount > 0)
      {
        entryCount--;
        free (entryTable[entryCount]);
      }
    free (op.entryInfo);
    (void) fclose (archiveFp);

    return EXIT_SUCCESS;
}


void
displaySplashScreen (void)
{
    (void) printf ("\n\n\n\n\t\t[::] Automotive Management System [::]\n\n");
    (void) printf ("\t\tCollege Software Engineering Project.\n\n");
    (void) printf ("\t\tWritten By:\tAristos Miliaressis\n");
    (void) printf ("\t\t\tAt:\tIek XINI Athens\n");
    (void) printf ("\t\t\tIn:\tStandard C 11\n");
    (void) printf ("\t\t\tDate:\t10/10/2015\n\n\t");
    pause ();
}


void
pause (void)
{
    (void) fflush (stdin);
    (void) printf ("\tPress [ Enter ] to continue...");
    (void) getchar ();
}


void
loadArchive (FILE *archiveFp)
{
    entry_t *entry = (entry_t*) calloc (1U, sizeof (entry_t));

    while (readEntry (archiveFp, entry))
      {
        entryTable[entryCount] = entry;
        entryCount++;
        entry = (entry_t*) calloc (1U, sizeof (entry_t));
      }

    free (entry);
    entryTable[entryCount] = NULL;
}


bool
goToMainMenu (FILE **archiveFp, operation_t *const op)
{
    clearScreen ();
    displayMainMenu ();
    char option = getUserOption ();

    clearScreen ();
    switch (option)
      {
      case '1':
        (void) printf ("\n\n\t[*] Display All (%zu) Entrys [*]\n\n", entryCount);

        displayEntryTable (entryTable);
        break;

      case '2':
        (void) printf ("\n\n\t[*] Create New Entry [*]\n\n");

        createNewEntry (op);
        break;

      case '3':
        (void) printf ("\n\n\t[*] Remove Entry [*]\n\n");

        removeEntry (op);
        break;

      case '4':
        (void) printf ("\n\n\t[*] Modify Entry [*]\n\n");

        modifyEntry (op);
        break;

      case '5':
        while ( goToSearchMenu () );
        break;

      case '6':
        (void) printf ("\n\n\tUpdating '%s' archive..", ARCHIVE);

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
    (void) printf ("\n\n\t[*] Main Menu [*]");
    (void) printf ("\n\n\t[1] - View All Entrys");
    (void) printf ("\n\t[2] - Create New Entry");
    (void) printf ("\n\t[3] - Remove Entry");
    (void) printf ("\n\t[4] - Modify Entry");
    (void) printf ("\n\t[5] - Search Archive");
    (void) printf ("\n\t[6] - Update Archive");
    (void) printf ("\n\t[0] - Exit");
    (void) printf ("\n\n\tSelect an Option (0-6): ");
    (void) fflush (stdin);
}


void
printAllIDs (void)
{
    int16_t *idList = (int16_t*) calloc (entryCount, 2U);
    size_t i;
    
    if (entryCount == 0 || idList == NULL)
        return;

    for (i = 0; i < entryCount; i++)
        idList[i] = atoi (entryTable[i]->id);

    quicksort (idList, 0, i-1);

    (void) printf ("\n\tIDs: %u", idList[0]);
    for (i = 1; i < entryCount; i++)
        (void) printf (", %u", idList[i]);
    
    free (idList);
    idList = NULL;
}


void
printAllColors (void)
{
    char **colorList = (char**) calloc (entryCount+1, sizeof (char*));
    size_t i;

    if (entryCount == 0 || colorList == NULL)
        return;

    for (i = 0; i <= entryCount; i++)
        colorList[i] = entryTable[i]->color;
    i--;

    i = removeDuplicateStr (colorList, i);

    (void) printf ("\n\tColors: %s", colorList[0]);
    for (; i > 0; --i)
        (void) printf (", %s", colorList[i]);
    
    free (colorList);
    colorList = NULL;
}


void
printAllManufacts (void)
{
    char **manufactList = (char**) calloc (entryCount+1, sizeof (char*));
    size_t i;

    if (entryCount == 0 || manufactList == NULL)
        return;

    for (i = 0; i <= entryCount; i++)
        manufactList[i] = entryTable[i]->manufact;
    i--;

    i = removeDuplicateStr (manufactList, i);

    (void) printf ("\n\tManufacturer: %s", manufactList[0]);
    for (; i > 0; --i)
        (void) printf (", %s", manufactList[i]);
    
    free (manufactList);
    manufactList = NULL;
}


void
printAllDates (void)
{
    int16_t *dateList = (int16_t*) calloc (entryCount, 2U);
    size_t i, j;
    
    if (entryCount == 0 || dateList == NULL)
        return;

    for (i = 0; i < entryCount; i++)
        dateList[i] = atoi (entryTable[i]->date);
    i--;

    i = removeDuplicate (dateList, i);

    quicksort (dateList, 0, i);

    (void) printf ("\n\tManufact Dates: %u", dateList[0]);
    for (j = 1; j <= i; j++)
        (void) printf (", %u", dateList[j]);
    
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
        (void) printf ("\n\n\tEntry created successfully!");
      }
    else
      {
        op->opType = INVALID;
        (void) printf ("\n\tError ID '%s' already in use..", op->entryInfo->id);
      }
    (void) getchar ();
}


void
removeEntry (operation_t *const op)
{
    printAllIDs ();

    (void) printf ("\n\n\tEnter car's ID number: ");
    (void) getString (op->entryInfo->id, sizeof (op->entryInfo->id));

    if (!lookupEntryByID (op->entryInfo->id, op->entryInfo))
      {
        op->opType = INVALID;
        (void) printf ("\n\tEntry ID '%s' doesn't exist..", op->entryInfo->id);
      }
    else
      {
        op->opType = REMOVE;
        (void) printf ("\n\tEntry removed successfully!");
      }
    (void) getchar ();
}


void
modifyEntry (operation_t *const op)
{
    printAllIDs ();

    (void) printf ("\n\n\tEnter car's ID number: ");
    (void) getString (op->entryInfo->id, sizeof (op->entryInfo->id));

    if (!lookupEntryByID (op->entryInfo->id, op->entryInfo))
      {
        op->opType = INVALID;
        (void) printf ("\n\tEntry ID '%s' doesn't exist..", op->entryInfo->id);
      }
    else
      {
        clearScreen ();
        (void) printf ("\n\n\t[*] Enter New Info [*]\n\n");
        displayTableHeader ();
        displayEntryRow (op->entryInfo);
        displayTableFooter ();
        printf ("\t");

        int i = atoi (op->entryInfo->id);
        (void) strcpy (op->oldId, op->entryInfo->id);

        enterCarInfo (op->entryInfo);
        if (!lookupEntryByID (op->entryInfo->id, NULL) || i == atoi (op->entryInfo->id))
          {
            op->opType = OVERWRITE;
            (void) printf ("\n\n\tEntry modified successfully!");
          }
        else
          {
            op->opType = INVALID;
            (void) printf ("\n\tError ID '%s' already in use..", op->entryInfo->id);
          }
      }
    (void) getchar ();
}


void
updateArchive (FILE **archiveFp, operation_t *const op)
{
    if (op->opType == APPEND)
      {
        writeToArchive (archiveFp, NULL, op->entryInfo);
        entryTable[entryCount] = (entry_t*) calloc (1U, sizeof (entry_t));
        copyEntry (entryTable[entryCount], op->entryInfo);
        entryCount++;
      }
    else if (op->opType == OVERWRITE)
      {
        writeToArchive (archiveFp, op->oldId, op->entryInfo);
        size_t index = (size_t) getIndexFromID (op->oldId);
        copyEntry (entryTable[index], op->entryInfo);
      }
    else if (op->opType == REMOVE)
      {
        writeToArchive (archiveFp, op->entryInfo->id, NULL);
        size_t index = (size_t) getIndexFromID (op->entryInfo->id);
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
    clearScreen ();
    displaySearchMenu ();
    char option = getUserOption ();

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

    char *searchKey = getSearchTerm ();
    entry_t **resultTable = findAllMatching (searchKey, option, searchKey[0]);

    clearScreen ();
    (void) printf ("\n\n\t[*] Search Results [*]\n\n");
    displayEntryTable (resultTable);

    free (resultTable);
    free (searchKey);
    resultTable = NULL;
    searchKey = NULL;

    return true;
}


char *
getSearchTerm (void)
{
    char *searchKey = (char*) calloc (1U, 32U);

    (void) printf ("\n\n\tEnter search term: ");
    (void) getString (searchKey, 32U);

    parseWhiteSpace (searchKey);
    strToUpper (searchKey);

    return searchKey;
}


void
displaySearchMenu (void)
{
    (void) printf ("\n\n\t[*] Search Archive [*]");
    (void) printf ("\n\n\t[1] - Search By ID");
    (void) printf ("\n\t[2] - Search By Color");
    (void) printf ("\n\t[3] - Search By Manufacturer");
    (void) printf ("\n\t[4] - Search By Date");
    (void) printf ("\n\t[0] - Back to Main Menu");
    (void) printf ("\n\n\tSelect an Option (0-4): ");
    (void) fflush (stdin);
}


void
displayTableHeader (void)
{
    (void) printf ("\t _______________________________________________________________\n");
    (void) printf ("\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n");
    (void) printf ("\t|---------------------------------------------------------------|");
}


void
displayTableFooter (void)
{
    (void) printf ("\n\t|_______________________________________________________________|\n\n");
}


char
getUserOption (void)
{
    char *str = (char*) calloc (1U, 30U);

    (void) getString (str, 30U);
    parseWhiteSpace (str);

    char ch = str[0];
    free (str);
    str = NULL;

    return ch;
}


void
enterCarInfo (entry_t *const entry)
{
    do
      {
        (void) printf ("\n\tEntry's ID (%u..%u): ", MIN_ID, MAX_ID);
        (void) getString (entry->id, sizeof (entry->id));

        if (isIntBetween (entry->id, MIN_ID, MAX_ID))
            break;

        (void) printf ("\n\tInvalid ID provided!");
        (void) printf ("\n\tAcceptable IDs are between %u and %u.\n",
                MIN_ID, MAX_ID);
      }
    while (true);

    (void) printf ("\n\n\tCar's Color: ");
    (void) getString (entry->color, sizeof (entry->color));
    strToUpper (entry->color);

    (void) printf ("\n\n\tCar's Manufacturer: ");
    (void) getString (entry->manufact, sizeof (entry->manufact));
    strToUpper (entry->manufact);

    do
      {
        (void) printf ("\n\n\tCar's date of manufacture (%u..%u): ",
                MIN_DATE, MAX_DATE);
        (void) getString (entry->date, sizeof (entry->date));

        if (isIntBetween (entry->date, MIN_DATE, MAX_DATE))
            break;

        (void) printf ("\n\tInvalid date provided!");
        (void) printf ("\n\tAcceptable dates are between %u and %u.",
                MIN_DATE, MAX_DATE);
      }
    while (true);
}


void
displayEntryRow (const entry_t *const entry)
{
    char tmp[30] = { 0 };

    (void) strcpy (tmp, entry->manufact);
    if (strlen (tmp) < 8)
        (void) strcat (tmp, "\t");

    (void) printf ("\n\t| %s\t\t%s\t\t%s\t\t%s\t|",
            entry->id, entry->color, tmp, entry->date);
}


void
displayEntryTable (entry_t *table[])
{
    size_t i = 0;

    displayTableHeader ();
    while (table[i] != NULL)
      {
        displayEntryRow (table[i]);
        i++;
      }

    if (i == 0U)
        (void) printf ("\n\t|                       No Entrys Found!                        |");

    displayTableFooter ();
    pause ();
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
lookupEntryByID (const char *const id, entry_t *const entry)
{
    for (size_t i = 0; i < entryCount; i++)
      {
        if (atoi (entryTable[i]->id) == atoi (id))
          {
            if (entry != NULL)
                copyEntry (entry, entryTable[i]);
            return true;
          }
      }

    return false;
}


void
copyEntry (entry_t *const dest, const entry_t *const src)
{
    (void) strcpy (dest->id, src->id);
    (void) strcpy (dest->color, src->color);
    (void) strcpy (dest->manufact, src->manufact);
    (void) strcpy (dest->date, src->date);
}


entry_t **
findAllMatching (char *key, const char col, const char operand)
{
    entry_t **resultTable = calloc (entryCount, sizeof (entry_t*));
    int16_t resultCount = 0;

    strToUpper (key);
    if ((col == ID_COL || col == DATE_COL) && operand != '\0')
        key++;

    for (size_t i = 0; i < entryCount; i++)
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


intmax_t
getIndexFromID (const char *const entryId)
{
  for (intmax_t index = 0; (size_t) index < entryCount; index++)
    {
      if (!strcmp (entryId, entryTable[index]->id))
          return index;
    }
    return -1;
}


void
writeToArchive (FILE **archiveFp,
                const char *const entryId,
                const entry_t *const entry)
{
    const char tmpFileName[] = "fRpZX6EPZV";
    entry_t tmpEntry = {{0}};

    /* Append Entry */
    if (entryId == NULL)
      {
        (void) fseek (*archiveFp, -1, SEEK_END);
        (void) fprintf (*archiveFp, "%s,%s,%s,%s,",
                entry->id,
                entry->color,
                entry->manufact,
                entry->date);
        (void) fflush (*archiveFp);
        return;
      }

    FILE *tmpFp = fopen (tmpFileName, "w");
    if (tmpFp == NULL)
        fatal ("opening temporary file");

    rewind (*archiveFp);
    while (readEntry (*archiveFp, &tmpEntry))
      {
        if (!strcmp (entryId, tmpEntry.id))
            break;

        (void) fprintf (tmpFp, "%s,%s,%s,%s,",
                 tmpEntry.id,
                 tmpEntry.color,
                 tmpEntry.manufact,
                 tmpEntry.date);
        (void) fflush (tmpFp);
      }

    /* Overwrite Entry */
    if (entry != NULL)
      {
        (void) fprintf (tmpFp, "%s,%s,%s,%s,",
                 entry->id,
                 entry->color,
                 entry->manufact,
                 entry->date);
        (void) fflush (tmpFp);
      }

    while (readEntry (*archiveFp, &tmpEntry))
      {
        (void) fprintf (tmpFp, "%s,%s,%s,%s,",
                 tmpEntry.id,
                 tmpEntry.color,
                 tmpEntry.manufact,
                 tmpEntry.date);
        (void) fflush (tmpFp);
      }
    (void) fclose (*archiveFp);
    (void) fclose (tmpFp);

    int ret = remove (ARCHIVE);
    if (ret == -1)
        fatal ("removing outdated archive");

    ret = rename (tmpFileName, ARCHIVE);
    if (ret == -1)
        fatal ("renaming archive");

    *archiveFp = fopen (ARCHIVE, "a+");
    if (*archiveFp == NULL)
        fatal ("reopening archive");
}

