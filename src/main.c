/* [08/30/2016 06:19:47 PM] */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "misc.h"


const char   ARCHIVE[]  = "ams_archive.csv";
const size_t SIZE_T_MAX = (size_t) -1;

#define COL_ID '1'
#define COL_COLOR '2'
#define COL_MANUFACT '3'
#define COL_DATE '4'

const uint16_t MIN_DATE = 1960U;
const uint16_t MAX_DATE = 2006U;
const uint16_t MIN_ID = 1000U;
const uint16_t MAX_ID = 9999U;

typedef struct  
{
    char id[5];
    char color[30];
    char manufact[30];
    char date[5];
} 
Car;


#define MAX_ENTRYS 512
Car *entryTable[MAX_ENTRYS];
size_t entryCount = 0U;

typedef enum { INVALID, REMOVE, OVERWRITE, APPEND } OperType;
typedef struct 
{
    OperType opType;
    Car *carInfo;
    char prevID[5];
} 
Operation;


void displaySplashScreen (void);
void displayMainMenu (void);
bool goToMainMenu (Operation *op);
void addCarEntry (Operation *op);
void removeCarEntry (Operation *op);
void modifyCarEntry (Operation *op);
void updateArchive (Operation *op);
void displayTable (Car *table[]);
void displayTableHeader (void);
void displayTableFooter (void);
void displayTableRow (const Car *entry);
void displaySearchMenu (void);
bool goToSearchMenu (void);
char *getSearchTerm (void);
char getUserOption (void);
void printAllColors (void);
void printAllManufacts (void);
void printAllIDs (void);
void printAllDates (void);
void enterCarInfo (Car *entry);
bool readNextEntry (FILE *archiveFp, Car *entry);
void loadArchive (void);
void copyEntry (Car *dest, const Car *src);
size_t getIndexFromID (const char *entryId);
bool lookupEntryByID (const char *id, Car *entry);
Car **findAllMatching (char *key, const char col, const char operand);
void writeNewArchive (const char *entryId, const Car *entry);
void appendToArchive (const Car *entry);
void replaceOldArchive (const char *oldArchive);


int
main (void)
{
    Operation op = {INVALID, NULL, {'\0'}};
    
    op.carInfo = (Car*) calloc (1U, sizeof (Car));
    if (op.carInfo == NULL)
        fatal ("allocating memory");
    loadArchive ();
    
    clearScreen ();
    displaySplashScreen ();

    while (goToMainMenu (&op));

    while (entryCount > 0)
      {
        entryCount--;
        free (entryTable[entryCount]);
      }
    free (op.carInfo);

    return EXIT_SUCCESS;
}


void
displaySplashScreen (void)
{
    printf ("\n\n\n\n\t\t[::] Automotive Management System [::]\n\n"\
            "\t\tCollege Software Engineering Project.\n\n"\
            "\t\tWritten By:\tAristos Miliaressis\n"\
            "\t\t\tAt:\tIek XINI Athens\n"\
            "\t\t\tIn:\tStandard C 11\n"\
            "\t\t\tDate:\t10/10/2015\n");
    fflush (stdout);
    pause ();
}


void
loadArchive (void)
{
    Car *entry = (Car*) calloc (1U, sizeof (Car));
    if (entry == NULL)
        fatal ("allocating memory");
    
    FILE *const archiveFp = fopen (ARCHIVE, "r");
    if (archiveFp == NULL)
        fatal ("openning archive");
    
    while (readNextEntry (archiveFp, entry))
      {
        entryTable[entryCount] = entry;
        entryCount++;
        entry = (Car*) calloc (1U, sizeof (Car));
      }

    fclose (archiveFp);
    free (entry);
    entryTable[entryCount] = NULL;
}


bool
goToMainMenu (Operation *const op)
{
    clearScreen ();
    displayMainMenu ();
    const char option = getUserOption ();

    clearScreen ();
    switch (option)
      {
      case '1':
        printf ("\n\n\t\t[*] Display All (%zu) Entrys [*]\n\n", entryCount);
        fflush (stdout);
              
        displayTable (entryTable);
        break;

      case '2':
        printf ("\n\n\t[*] Create New Car [*]\n");
        fflush (stdout);
              
        addCarEntry (op);
        break;

      case '3':
        printf ("\n\n\t[*] Remove Car [*]\n\n");
        fflush (stdout);

        removeCarEntry (op);
        break;

      case '4':
        printf ("\n\n\t[*] Modify Car [*]\n\n");
        fflush (stdout);

        modifyCarEntry (op);
        break;

      case '5':
        while ( goToSearchMenu () );
        break;

      case '6':
        printf ("\n\n\tUpdating '%s' archive..", ARCHIVE);
        fflush (stdout);

        updateArchive (op);
        break;

      case '0':
        return false;
      }

    return true;
}


void
displayMainMenu (void)
{
    printf ("\n\n\t[*] Main Menu [*]"\
            "\n\n\t[1] - View All Entrys"\
            "\n\t[2] - Create New Car"\
            "\n\t[3] - Remove Car"\
            "\n\t[4] - Modify Car"\
            "\n\t[5] - Search Archive"\
            "\n\t[6] - Update Archive"\
            "\n\t[0] - Exit"\
            "\n\n\tSelect an Option (0-6): ");
    fflush (stdout);
}


void
printAllIDs (void)
{
    int16_t *idList = (int16_t*) calloc (entryCount, 2U);
    size_t i;
    
    if (entryCount == 0 || idList == NULL)
        return;

    for (i = 0; i < entryCount; i++)
        idList[i] = (int16_t) atoi (entryTable[i]->id);

    quicksort (idList, 0, i - 1);

    printf ("\n\tIDs: %d", idList[0]);
    fflush (stdout);
    
    for (i = 1; i < entryCount; i++)
      {
        printf (", %d", idList[i]);
        fflush (stdout);
      }
    
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

    printf ("\n\tColors: %s", colorList[0]);
    fflush (stdout);
    
    for (; i > 0; --i)
      {
        printf (", %s", colorList[i]);
        fflush (stdout);
      }
    
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

    printf ("\n\tManufacturer: %s", manufactList[0]);
    fflush (stdout);
    
    for (; i > 0; --i)
      {
        printf (", %s", manufactList[i]);
        fflush (stdout);
      }
    
    free (manufactList);
    manufactList = NULL;
}


void
printAllDates (void)
{
    int16_t *dateList = (int16_t*) calloc (entryCount, 2U);
    size_t i;
    
    if (entryCount == 0 || dateList == NULL)
        return;

    for (i = 0; i < entryCount; i++)
        dateList[i] = (int16_t) atoi (entryTable[i]->date);
    i--;

    i = removeDuplicateInt (dateList, i);

    quicksort (dateList, 0, i);

    printf ("\n\tManufact Dates: %d", dateList[0]);
    fflush (stdout);
    
    for (size_t j = 1; j <= i; j++)
      {
        printf (", %d", dateList[j]);
        fflush (stdout);
      }
    
    free (dateList);
    dateList = NULL;
}


void
addCarEntry (Operation *const op)
{
    enterCarInfo (op->carInfo);

    if (getIndexFromID (op->carInfo->id) == SIZE_T_MAX)
      {
        op->opType = APPEND;
        printf ("\n\n\tEntry created successfully!");
      }
    else
      {
        op->opType = INVALID;
        printf ("\n\tError ID '%s' already in use..", op->carInfo->id);
      }
    fflush (stdout);
    flush_stdin ();
}


void
removeCarEntry (Operation *const op)
{
    printAllIDs ();

    printf ("\n\n\tEnter car's ID number: ");
    fflush (stdout);
    
    getString (op->carInfo->id, sizeof (op->carInfo->id));
    if (!lookupEntryByID (op->carInfo->id, op->carInfo))
      {
        op->opType = INVALID;
        printf ("\n\tEntry ID '%s' doesn't exist..", op->carInfo->id);
      }
    else
      {
        op->opType = REMOVE;
        printf ("\n\tEntry removed successfully!");
      }
    fflush (stdout);
    flush_stdin ();
}


void
modifyCarEntry (Operation *const op)
{
    printAllIDs ();

    printf ("\n\n\tEnter car's ID number: ");
    fflush (stdout);
    
    getString (op->carInfo->id, sizeof (op->carInfo->id));
    if (!lookupEntryByID (op->carInfo->id, op->carInfo))
      {
        op->opType = INVALID;
        printf ("\n\tEntry ID '%s' doesn't exist..", op->carInfo->id);
      }
    else
      {
        clearScreen ();
        printf ("\n\n\t\t[*] Enter New Info [*]\n\n");
        fflush (stdout);
          
        displayTableHeader ();
        displayTableRow (op->carInfo);
        displayTableFooter ();

        int i = atoi (op->carInfo->id);
        strcpy (op->prevID, op->carInfo->id);

        enterCarInfo (op->carInfo);
        if (getIndexFromID (op->carInfo->id) == SIZE_T_MAX ||
            i == atoi (op->carInfo->id))
          {
            op->opType = OVERWRITE;
            printf ("\n\n\tEntry modified successfully!");
          }
        else
          {
            op->opType = INVALID;
            printf ("\n\tError ID '%s' already in use..", op->carInfo->id);
          }
      }
    fflush (stdout);
    flush_stdin ();
}


void
updateArchive (Operation *const op)
{
    if (op->opType == APPEND)
      {
        appendToArchive (op->carInfo);
        entryTable[entryCount] = (Car*) calloc (1U, sizeof (Car));
        copyEntry (entryTable[entryCount], op->carInfo);
        entryCount++;
      }
    else if (op->opType == OVERWRITE)
      {
        writeNewArchive (op->prevID, op->carInfo);
        const size_t index = getIndexFromID (op->prevID);
        copyEntry (entryTable[index], op->carInfo);
      }
    else if (op->opType == REMOVE)
      {
        writeNewArchive (op->carInfo->id, NULL);
        size_t index = getIndexFromID (op->carInfo->id);
        free (entryTable[index]);
        for (; index <= entryCount; index++)
            entryTable[index] = entryTable[index+1];
        entryCount--;
      }

    printf ("\n\n\tUpdated archive successfully!");
    fflush (stdout);
    
    flush_stdin ();
}


bool
goToSearchMenu (void)
{
    clearScreen ();
    displaySearchMenu ();
    const char option = getUserOption ();

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
    Car **resultTable = findAllMatching (searchKey, option, searchKey[0]);

    clearScreen ();
    printf ("\n\n\t\t[*] Search Results [*]\n\n");
    fflush (stdout);
    displayTable (resultTable);

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

    printf ("\n\n\tEnter search term: ");
    fflush (stdout);
    
    getString (searchKey, 32U);
    parseWhiteSpace (searchKey);

    return strToUpper (searchKey);
}


void
displaySearchMenu (void)
{
    printf ("\n\n\t[*] Search Archive [*]"\
            "\n\n\t[1] - Search By ID"\
            "\n\t[2] - Search By Color"\
            "\n\t[3] - Search By Manufacturer"\
            "\n\t[4] - Search By Date"\
            "\n\t[0] - Back to Main Menu"\
            "\n\n\tSelect an Option (0-4): ");
    fflush (stdout);
}


void
displayTableHeader (void)
{
    printf ("\t\t ________________________________"\
            "_______________________________\n"\
            "\t\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n"\
            "\t\t|-----------------------------"\
            "----------------------------------|");
    fflush (stdout);
}


void
displayTableFooter (void)
{
    printf ("\n\t\t|____________________________"\
            "___________________________________|\n");
    fflush (stdout);
}


char
getUserOption (void)
{
    char *str = (char*) calloc (1U, 30U);

    getString (str, 30U);
    parseWhiteSpace (str);

    const char ch = str[0];
    free (str);
    str = NULL;

    return ch;
}


void
enterCarInfo (Car *const entry)
{
    do
      {
        printf ("\n\n\tEntry's ID (%u..%u): ", MIN_ID, MAX_ID);
        fflush (stdout);
          
        getString (entry->id, sizeof (entry->id));
        if (isIntBetween (entry->id, MIN_ID, MAX_ID))
            break;

        printf ("\n\tInvalid ID provided!"\
                "\n\tAcceptable IDs are between %u and %u.\n", 
                MIN_ID, MAX_ID);
        fflush (stdout);
      }
    while (true);

    printf ("\n\n\tCar's Color: ");
    fflush (stdout);
    
    getString (entry->color, sizeof (entry->color));
    strToUpper (entry->color);

    printf ("\n\n\tCar's Manufacturer: ");
    fflush (stdout);
    
    getString (entry->manufact, sizeof (entry->manufact));
    strToUpper (entry->manufact);

    do
      {
        printf ("\n\n\tCar's date of manufacture (%u..%u): ",
                MIN_DATE, MAX_DATE);
        fflush (stdout);
        getString (entry->date, sizeof (entry->date));

        if (isIntBetween (entry->date, MIN_DATE, MAX_DATE))
            break;

        printf ("\n\tInvalid date provided!"\
                "\n\tAcceptable dates are between %u and %u.", 
                MIN_DATE, MAX_DATE);
        fflush (stdout);
      }
    while (true);
}


void
displayTableRow (const Car *const entry)
{
    char tmp[30] = { 0 };

    strcpy (tmp, entry->manufact);
    if (strlen (tmp) < 8)
        strcat (tmp, "\t");

    printf ("\n\t\t| %s\t\t%s\t\t%s\t\t%s\t|",
            entry->id, entry->color, tmp, entry->date);
    fflush (stdout);
}


void
displayTable (Car **table)
{
    size_t i = 0;

    displayTableHeader ();
    while (table[i] != NULL)
      {
        displayTableRow (table[i]);
        i++;
      }

    if (i == 0U)
      {
        printf ("\n\t|                       No Entrys Found!"\
                "                        |");
        fflush (stdout);
      }

    displayTableFooter ();
    pause ();
}


bool
readNextEntry (FILE *archiveFp, Car *const entry)
{
    return !(  !csvReadNextVal (archiveFp, entry->id)
            || !csvReadNextVal (archiveFp, entry->color)
            || !csvReadNextVal (archiveFp, entry->manufact)
            || !csvReadNextVal (archiveFp, entry->date)
           );
}


bool
lookupEntryByID (const char *const id, Car *const entry)
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
copyEntry (Car *const dest, const Car *const src)
{
    strcpy (dest->id, src->id);
    strcpy (dest->color, src->color);
    strcpy (dest->manufact, src->manufact);
    strcpy (dest->date, src->date);
}


Car **
findAllMatching (char *key, const char col, const char operand)
{
    Car **resultTable = (Car**) calloc (entryCount, sizeof (Car*));
    size_t resultCount = 0;

    strToUpper (key);
    if ((col == COL_ID || col == COL_DATE) && operand != '\0')
        key++;

    for (size_t i = 0; i < entryCount; i++)
      {
        switch (col)
          {
          case COL_ID:
            {
              if ((operand == '>'  && atoi (entryTable[i]->id) > atoi (key)) ||
                  (operand == '<'  && atoi (entryTable[i]->id) < atoi (key)) ||
                  (operand == '\0' && atoi (entryTable[i]->id) == atoi (key)))
                {
                  resultTable[resultCount] = entryTable[i];
                  resultCount++;
                }
              break;
            }
          case COL_COLOR:
            {
              if (strncmp (key, entryTable[i]->color, strlen (key)) == 0)
                {
                  resultTable[resultCount] = entryTable[i];
                  resultCount++;
                }
              break;
            }
          case COL_MANUFACT:
            {
              if (strncmp (key, entryTable[i]->manufact, strlen (key)) == 0)
                {
                  resultTable[resultCount] = entryTable[i];
                  resultCount++;
                }
              break;
            }
          case COL_DATE:
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
  for (size_t index = 0; index < entryCount; index++)
    {
      if (!strcmp (entryId, entryTable[index]->id))
          return index;
    }
    return SIZE_T_MAX;
}


void 
appendToArchive (const Car *const entry)
{
    FILE *const archiveFp = fopen (ARCHIVE, "a");
    if (archiveFp == NULL)
        fatal ("openning archive");
    
    fprintf (archiveFp, "%s,%s,%s,%s,",
            entry->id, entry->color,
            entry->manufact, entry->date);
    
    fflush (archiveFp);
    fclose (archiveFp);
}


void
writeNewArchive (const char *const entryId, const Car *const entry)
{
    const char tmpFileName[] = "ams_archive.tmp";
    Car tmpEntry = {{0}, {0}, {0}, {0}};

    FILE *const archiveFp = fopen (ARCHIVE, "r");
    if (archiveFp == NULL)
        fatal ("opening archive");
    
    FILE *const tmpFp = fopen (tmpFileName, "w");
    if (tmpFp == NULL)
        fatal ("opening temporary file");

    /* Copy up to subject entry */
    rewind (archiveFp);
    while (readNextEntry (archiveFp, &tmpEntry))
      {
        if (!strcmp (entryId, tmpEntry.id))
            break;

        fprintf (tmpFp, "%s,%s,%s,%s,",
                 tmpEntry.id, tmpEntry.color,
                 tmpEntry.manufact, tmpEntry.date);
        fflush (tmpFp);
      }

    /* Write modified entry */
    if (entry != NULL)
      {
        fprintf (tmpFp, "%s,%s,%s,%s,",
                 entry->id, entry->color,
                 entry->manufact, entry->date);
        fflush (tmpFp);
      }

    /* Copy remaining entrys */
    while (readNextEntry (archiveFp, &tmpEntry))
      {
        fprintf (tmpFp, "%s,%s,%s,%s,",
                 tmpEntry.id, tmpEntry.color,
                 tmpEntry.manufact, tmpEntry.date);
        fflush (tmpFp);
      }
    fclose (archiveFp);
    fclose (tmpFp);
    
    replaceOldArchive (tmpFileName);
}


void 
replaceOldArchive (const char *const oldArchive)
{
    int ret = remove (ARCHIVE);
    if (ret == -1)
        fatal ("removing outdated archive");

    ret = rename (oldArchive, ARCHIVE);
    if (ret == -1)
        fatal ("renaming archive");
}

