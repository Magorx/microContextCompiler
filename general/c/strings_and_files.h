#ifndef GENERAL_STRINGS_AND_FILES
#define GENERAL_STRINGS_AND_FILES

#include "common.h"

#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>

//=============================================================================
//<KCTF> Strings_and_files ====================================================

typedef struct Line_s {
    unsigned char *string;
    size_t len;
} Line;

/// Struct to store file's information into
typedef struct File_s {
    const char *name;
    FILE *ptr;
    struct stat info;
    unsigned char *text;
    size_t lines_cnt;
    Line **lines;

    unsigned char *cc; // cur char
} File;

/**
    \brief Quadratic sort

    Sorts array on-place with given comparator

    \param[in] arr array which needs to be sorted
    \param[in] elem_cnt count of elements, [0, elem_cnt) will be sorted
    \param[in] elem_size size of each element in bytes
    \param[in] comp comparator returning an int <0 of elem1<elem2, 0 if elem1=elem2, >0 if elem1>elem2
*/
void qqh_sort(void *arr, const size_t elem_cnt, const size_t elem_size, int (*comp)(const void *elem1, const void *elem2));

/**
    \brief Comparator for two lines

    Ignores everything that is not a Russian or English letter

    \param[in] elem1,elem2 elements to compare
    \return an int <0 if elem1<elem2, 0 if elem1=elem2, >0 if elem1>elem2
*/
int compare_lines_letters(const void *elem1, const void *elem2);

/**
    \brief Reversed comparator for two lines

    Ignores everything that is not a Russian or English letter

    \param[in] elem1,elem2 elements to compare
    \return an int >0 if elem1<elem2, 0 if elem1=elem2, 0 if elem1>elem2
*/
int reverse_compare_lines_letters(const void **elem1, const void **elem2);

/**
    \brief Calls all necessary free's

    Kind of destructor for the File structure

    \param[in] file object to be destroyed
*/
void File_destruct(const File *file);

/**
    \brief Reads file

    Estimates file's size and allocates just enough memory + 1 byte for \0, then calls read_lines to fill buffer 'text' and index 'lines'

    \param[in] file object to be read to
    \param[in] name - filename to be read from
    \return 0 if file is read successfully, else error code <0
*/
int File_construct(File *file, const char *name);

/**
    \brief Reads lines from file

    Stores them into given File object

    \param[in] file an object to write into, contains input file name
    \return 0 if file is read successfully, else error code <0
*/
int read_lines(File *file);

/**
    \brief Prints file into given file



    \param[in] file file containing text to write
    \param[in] output file name
*/
void print_file(const File *file, const char *fout_name, const char *mode);


/**
    \brief Checks if c is a Russian or an English letter

    .

    \param[in] c char to check
    \return true if c is a Russian or an English letter, else false
*/
int is_countable(const unsigned char c);

/**
    \brief Swaps contains of two ptrs

    .

    \param[in] first,second prts to swap
    \return
*/
void swap_ptrs(void **first, void **second);

/**
    \brief Sets *c on next position of the same string, where is_countable returns true.

    If there are no such left, *c will be pointer to the end of the string ('\0')

    \param[in] first,second prts to swap
    \return
*/
void UChar_get_next_symb(unsigned char **c);
void Char_get_next_symb(char **c);

char Char_in_string(const unsigned char c, const unsigned char *str);

/**
    \brief Prints text error message to standard output

    .

    \param[in] error code of error to print
*/
void print_error(int error);

#endif // GENERAL_STRINGS_AND_FILES
