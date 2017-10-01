/* [30/08/2016 06:19:47 PM] */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "misc.h"


const uint16_t MIN_DATE = 1960U;
const uint16_t MAX_DATE = 2006U;
const uint16_t MIN_ID = 1000U;
const uint16_t MAX_ID = 9999U;

typedef enum { ID = '1', COLOR = '2', MANUFACT = '3', DATE = '4' } TableColumn;
typedef struct
{
    char id[5];
    char color[30];
    char manufact[30];
    char date[5];
}
Car;

const char ARCHIVE[] = "ams_archive.csv";

#define MAX_ENTRYS 512 
Car *entry_table[MAX_ENTRYS + 1]; 
size_t entry_count = 0U;

typedef enum { INVALID, REMOVE, OVERWRITE, APPEND } OperType;
typedef struct
{
    OperType type;
    Car *data;
    char prev_id[5];
}
Operation;


void display_splash_screen (void);
void display_main_menu (void);
bool goto_main_menu (Operation *op);
void add_car_entry (Operation *op);
void remove_car_entry (Operation *op);
void modify_car_entry (Operation *op);
void update_archive (Operation *op);
void display_table (Car *table[]);
void display_table_header (void);
void display_table_footer (void);
void display_table_row (const Car *entry);
void display_search_menu (void);
bool goto_search_menu (void);
char *get_search_term (void);
char get_user_option (void);
void print_all_colors (void);
void print_all_manufacts (void);
void print_all_ids (void);
void print_all_dates (void);
void enter_car_info (Car *entry);
bool read_next_entry (FILE *archive, Car *entry);
void load_archive (void);
void copy_entry (Car *dest, const Car *src);
size_t get_index (const char *id);
bool lookup_entry (const char *id, Car *entry);
Car **find_all_matching (char *key, TableColumn col, const char operand);
void write_new_archive (const char *id, const Car *entry);
void append_to_archive (const Car *entry);
void replace_old_archive (const char *old_archive);


int
main (void)
{
    Operation op = {INVALID, NULL, {'\0'}};

    op.data = (Car*) malloc (sizeof (Car));
    if (op.data == NULL)
        fatal ("allocating memory");
    load_archive ();

    clear_console ();
    display_splash_screen ();

    while (goto_main_menu (&op));

    while (entry_count > 0)
      {
        entry_count--;
        free (entry_table[entry_count]);
      }
    free (op.data);

    return EXIT_SUCCESS;
}


void
display_splash_screen (void)
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
load_archive (void)
{
    Car *entry = (Car*) malloc (sizeof (Car));
    if (entry == NULL)
        fatal ("allocating memory");

    FILE *const archive = fopen (ARCHIVE, "r");
    if (archive == NULL)
        fatal ("openning archive");

    while (read_next_entry (archive, entry))
      {
        entry_table[entry_count] = entry;
        entry_count++;
        entry = (Car*) malloc (sizeof (Car));
        if (entry_count == MAX_ENTRYS)
        	break;
      }

    fclose (archive);
    free (entry);
    entry_table[entry_count] = NULL;
}


bool
goto_main_menu (Operation *const op)
{
    clear_console ();
    display_main_menu ();
    const char option = get_user_option ();

    clear_console ();
    switch (option)
      {
      case '1':
        printf ("\n\n\t\t[*] Display All (%zu) Entrys [*]\n\n", entry_count);
        fflush (stdout);

        display_table (entry_table);
        return true;

      case '2':
        printf ("\n\n\t[*] Create New Car [*]\n");
        fflush (stdout);

        add_car_entry (op);
        return true;

      case '3':
        printf ("\n\n\t[*] Remove Car [*]\n\n");
        fflush (stdout);

        remove_car_entry (op);
        return true;

      case '4':
        printf ("\n\n\t[*] Modify Car [*]\n\n");
        fflush (stdout);

        modify_car_entry (op);
        return true;

      case '5':
        while ( goto_search_menu () );
        return true;

      case '6':
        printf ("\n\n\tUpdating '%s' archive..", ARCHIVE);
        fflush (stdout);

        update_archive (op);
        return true;

      case '0':
        return false;

      default:
        return true;
      }
}


void
display_main_menu (void)
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
print_all_ids (void)
{
    int16_t *id_list = (int16_t*) calloc (entry_count, 2U);
    size_t i;

    if (entry_count == 0 || id_list == NULL)
        return;

    for (i = 0; i < entry_count; i++)
        id_list[i] = (int16_t) atoi (entry_table[i]->id);

    quicksort (id_list, 0, (intmax_t) i - 1);

    printf ("\n\tIDs: %d", id_list[0]);
    fflush (stdout);

    for (i = 1; i < entry_count; i++)
      {
        printf (", %d", id_list[i]);
        fflush (stdout);
      }

    free (id_list);
    id_list = NULL;
}


void
print_all_colors (void)
{
    char **color_list = (char**) calloc (entry_count + 1, sizeof (char*));
    size_t i;

    if (entry_count == 0 || color_list == NULL)
        return;

    for (i = 0; i <= entry_count; i++)
        color_list[i] = entry_table[i]->color;
    i--;

    i = remove_dup_str (color_list, i);

    printf ("\n\tColors: %s", color_list[0]);
    fflush (stdout);

    for (; i > 0; --i)
      {
        printf (", %s", color_list[i]);
        fflush (stdout);
      }

    free (color_list);
    color_list = NULL;
}


void
print_all_manufacts (void)
{
    char **manufact_list = (char**) calloc (entry_count + 1, sizeof (char*));
    size_t i;

    if (entry_count == 0 || manufact_list == NULL)
        return;

    for (i = 0; i <= entry_count; i++)
        manufact_list[i] = entry_table[i]->manufact;
    i--;

    i = remove_dup_str (manufact_list, i);

    printf ("\n\tManufacturer: %s", manufact_list[0]);
    fflush (stdout);

    for (; i > 0; --i)
      {
        printf (", %s", manufact_list[i]);
        fflush (stdout);
      }

    free (manufact_list);
    manufact_list = NULL;
}


void
print_all_dates (void)
{
    int16_t *date_list = (int16_t*) calloc (entry_count, 2U);
    size_t i;

    if (entry_count == 0 || date_list == NULL)
        return;

    for (i = 0; i < entry_count; i++)
        date_list[i] = (int16_t) atoi (entry_table[i]->date);
    i--;

    i = remove_dup_int (date_list, i);

    quicksort (date_list, 0, (intmax_t) i);

    printf ("\n\tManufact Dates: %d", date_list[0]);
    fflush (stdout);

    for (size_t j = 1; j <= i; j++)
      {
        printf (", %d", date_list[j]);
        fflush (stdout);
      }

    free (date_list);
    date_list = NULL;
}


void
add_car_entry (Operation *const op)
{
    enter_car_info (op->data);

    if (get_index (op->data->id) == SIZE_T_MAX)
      {
        op->type = APPEND;
        printf ("\n\n\tEntry created successfully!");
      }
    else
      {
        op->type = INVALID;
        printf ("\n\tError ID '%s' already in use..", op->data->id);
      }
    fflush (stdout);
    flush_stdin ();
}


void
remove_car_entry (Operation *const op)
{
    print_all_ids ();

    printf ("\n\n\tEnter car's ID number: ");
    fflush (stdout);

    get_string (op->data->id, sizeof (op->data->id));
    if (!lookup_entry (op->data->id, op->data))
      {
        op->type = INVALID;
        printf ("\n\tEntry ID '%s' doesn't exist..", op->data->id);
      }
    else
      {
        op->type = REMOVE;
        printf ("\n\tEntry removed successfully!");
      }
    fflush (stdout);
    flush_stdin ();
}


void
modify_car_entry (Operation *const op)
{
    print_all_ids ();

    printf ("\n\n\tEnter car's ID number: ");
    fflush (stdout);

    get_string (op->data->id, sizeof (op->data->id));
    if (!lookup_entry (op->data->id, op->data))
      {
        op->type = INVALID;
        printf ("\n\tEntry ID '%s' doesn't exist..", op->data->id);
      }
    else
      {
        clear_console ();
        printf ("\n\n\t\t[*] Enter New Info [*]\n\n");
        fflush (stdout);

        display_table_header ();
        display_table_row (op->data);
        display_table_footer ();

        int i = atoi (op->data->id);
        strcpy (op->prev_id, op->data->id);

        enter_car_info (op->data);
        if (get_index (op->data->id) == SIZE_T_MAX ||
            i == atoi (op->data->id))
          {
            op->type = OVERWRITE;
            printf ("\n\n\tEntry modified successfully!");
          }
        else
          {
            op->type = INVALID;
            printf ("\n\tError ID '%s' already in use..", op->data->id);
          }
      }
    fflush (stdout);
    flush_stdin ();
}


void
update_archive (Operation *const op)
{
    if (op->type == APPEND)
      {
      	if (entry_count == MAX_ENTRYS)
      	  {
      	  	printf ("\n\n\tError maximum number of entrys reached."\
      	  			"\n\n\tFailed to update archive!");
      	  	fflush (stdout);

      	  	flush_stdin ();
      	  	return;
      	  }

        append_to_archive (op->data);
        entry_table[entry_count] = (Car*) malloc (sizeof (Car));
        copy_entry (entry_table[entry_count], op->data);
        entry_count++;
      }
    else if (op->type == OVERWRITE)
      {
        write_new_archive (op->prev_id, op->data);
        const size_t index = get_index (op->prev_id);
        copy_entry (entry_table[index], op->data);
      }
    else if (op->type == REMOVE)
      {
        write_new_archive (op->data->id, NULL);
        size_t index = get_index (op->data->id);
        free (entry_table[index]);
        for (; index <= entry_count; index++)
            entry_table[index] = entry_table[index+1];
        entry_count--;
      }

    printf ("\n\n\tUpdated archive successfully!");
    fflush (stdout);

    flush_stdin ();
}


bool
goto_search_menu (void)
{
    clear_console ();
    display_search_menu ();
    const char option = get_user_option ();

    switch (option)
      {
      case '0':
          return false;
      case '1':
          print_all_ids ();
          break;
      case '2':
          print_all_colors ();
          break;
      case '3':
          print_all_manufacts ();
          break;
      case '4':
          print_all_dates ();
          break;
      default:
          return true;
      }

    char *key = get_search_term ();
    Car **results = find_all_matching (key, option, key[0]);

    clear_console ();
    printf ("\n\n\t\t[*] Search Results [*]\n\n");
    fflush (stdout);
    display_table (results);

    free (results);
    free (key);
    results = NULL;
    key = NULL;

    return true;
}


char *
get_search_term (void)
{
    char *term = (char*) malloc (32U);

    printf ("\n\n\tEnter search term: ");
    fflush (stdout);

    get_string (term, 32U);
    strip_white_space (term);

    return str_to_upper (term);
}


void
display_search_menu (void)
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
display_table_header (void)
{
    printf ("\t\t ________________________________"\
            "_______________________________\n"\
            "\t\t| ID  \t\tColor\t\tManufacturer\t\tDate\t|\n"\
            "\t\t|-----------------------------"\
            "----------------------------------|");
    fflush (stdout);
}


void
display_table_footer (void)
{
    printf ("\n\t\t|____________________________"\
            "___________________________________|\n");
    fflush (stdout);
}


char
get_user_option (void)
{
    char *str = (char*) malloc (30U);

    get_string (str, 30U);
    strip_white_space (str);

    const char ch = str[0];
    free (str);
    str = NULL;

    return ch;
}


void
enter_car_info (Car *const entry)
{
    do
      {
        printf ("\n\n\tEntry's ID (%u..%u): ", MIN_ID, MAX_ID);
        fflush (stdout);

        get_string (entry->id, sizeof (entry->id));
        if (is_in_range (entry->id, MIN_ID, MAX_ID))
            break;

        printf ("\n\tInvalid ID provided!"\
                "\n\tAcceptable IDs are between %u and %u.\n",
                MIN_ID, MAX_ID);
        fflush (stdout);
      }
    while (true);

    printf ("\n\n\tCar's Color: ");
    fflush (stdout);

    get_string (entry->color, sizeof (entry->color));
    str_to_upper (entry->color);

    printf ("\n\n\tCar's Manufacturer: ");
    fflush (stdout);

    get_string (entry->manufact, sizeof (entry->manufact));
    str_to_upper (entry->manufact);

    do
      {
        printf ("\n\n\tCar's date of manufacture (%u..%u): ",
                MIN_DATE, MAX_DATE);
        fflush (stdout);
        get_string (entry->date, sizeof (entry->date));

        if (is_in_range (entry->date, MIN_DATE, MAX_DATE))
            break;

        printf ("\n\tInvalid date provided!"\
                "\n\tAcceptable dates are between %u and %u.",
                MIN_DATE, MAX_DATE);
        fflush (stdout);
      }
    while (true);
}


void
display_table_row (const Car *const entry)
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
display_table (Car **table)
{
    size_t i = 0;

    display_table_header ();
    while (table[i] != NULL)
      {
        display_table_row (table[i]);
        i++;
      }

    if (i == 0U)
      {
        printf ("\n\t|                       No Entrys Found!"\
                "                        |");
        fflush (stdout);
      }

    display_table_footer ();
    pause ();
}


bool
read_next_entry (FILE *archive, Car *const entry)
{
    return !(  !csv_read_next (archive, entry->id)
            || !csv_read_next (archive, entry->color)
            || !csv_read_next (archive, entry->manufact)
            || !csv_read_next (archive, entry->date)
           );
}


bool
lookup_entry (const char *const id, Car *const entry)
{
    for (size_t i = 0; i < entry_count; i++)
      {
        if (atoi (entry_table[i]->id) == atoi (id))
          {
            if (entry != NULL)
                copy_entry (entry, entry_table[i]);
            return true;
          }
      }

    return false;
}


void
copy_entry (Car *const dest, const Car *const src)
{
    strcpy (dest->id, src->id);
    strcpy (dest->color, src->color);
    strcpy (dest->manufact, src->manufact);
    strcpy (dest->date, src->date);
}


Car **
find_all_matching (char *key, TableColumn col, const char operand)
{
    Car **results = (Car**) calloc (entry_count, sizeof (Car*));
    size_t count = 0;

    str_to_upper (key);
    if ((col == ID || col == DATE) && operand != '\0')
        key++;

    for (size_t i = 0; i < entry_count; i++)
      {
        switch (col)
          {
          case ID:
            {
              if ((operand == '>'  && atoi (entry_table[i]->id) > atoi (key)) ||
                  (operand == '<'  && atoi (entry_table[i]->id) < atoi (key)) ||
                  (operand == '\0' && atoi (entry_table[i]->id) == atoi (key)))
                {
                  results[count] = entry_table[i];
                  count++;
                }
              break;
            }
          case COLOR:
            {
              if (strncmp (key, entry_table[i]->color, strlen (key)) == 0)
                {
                  results[count] = entry_table[i];
                  count++;
                }
              break;
            }
          case MANUFACT:
            {
              if (strncmp (key, entry_table[i]->manufact, strlen (key)) == 0)
                {
                  results[count] = entry_table[i];
                  count++;
                }
              break;
            }
          case DATE:
            {
              if ((operand == '>' && atoi (entry_table[i]->date) > atoi (key)) ||
                  (operand == '<' && atoi (entry_table[i]->date) < atoi (key)))
                {
                  results[count] = entry_table[i];
                  count++;
                }
            }
          }
      }

    return results;
}


size_t
get_index (const char *const id)
{
  for (size_t index = 0; index < entry_count; index++)
    {
      if (!strcmp (id, entry_table[index]->id))
          return index;
    }
    return SIZE_T_MAX;
}


void
append_to_archive (const Car *const entry)
{
    FILE *const archive = fopen (ARCHIVE, "a");
    if (archive == NULL)
        fatal ("openning archive");

    fprintf (archive, "%s,%s,%s,%s,",
            entry->id, entry->color,
            entry->manufact, entry->date);

    fflush (archive);
    fclose (archive);
}


void
write_new_archive (const char *const id, const Car *const entry)
{
    const char tmp_file_name[] = "ams_archive.tmp";
    Car tmp_entry = {{0}, {0}, {0}, {0}};

    FILE *const archive = fopen (ARCHIVE, "r");
    if (archive == NULL)
        fatal ("opening archive");

    FILE *const tmp_archive = fopen (tmp_file_name, "w");
    if (tmp_archive == NULL)
        fatal ("opening temporary file");

    /* Copy up to subject entry */
    rewind (archive);
    while (read_next_entry (archive, &tmp_entry))
      {
        if (!strcmp (id, tmp_entry.id))
            break;

        fprintf (tmp_archive, "%s,%s,%s,%s,",
                 tmp_entry.id, tmp_entry.color,
                 tmp_entry.manufact, tmp_entry.date);
        fflush (tmp_archive);
      }

    /* Write modified entry */
    if (entry != NULL)
      {
        fprintf (tmp_archive, "%s,%s,%s,%s,",
                 entry->id, entry->color,
                 entry->manufact, entry->date);
        fflush (tmp_archive);
      }

    /* Copy remaining entrys */
    while (read_next_entry (archive, &tmp_entry))
      {
        fprintf (tmp_archive, "%s,%s,%s,%s,",
                 tmp_entry.id, tmp_entry.color,
                 tmp_entry.manufact, tmp_entry.date);
        fflush (tmp_archive);
      }
    fclose (archive);
    fclose (tmp_archive);

    replace_old_archive (tmp_file_name);
}


void
replace_old_archive (const char *const old_archive)
{
    int ret = remove (ARCHIVE);
    if (ret == -1)
        fatal ("removing outdated archive");

    ret = rename (old_archive, ARCHIVE);
    if (ret == -1)
        fatal ("renaming archive");
}

