#include "strings_and_files.h"

int is_countable(const unsigned char c) {
    return isalnum(c);
}

void swap_ptrs(void **first, void **second) {
    assert(first);
    assert(second);

    void *tmp = *second;
    *second = *first;
    *first = tmp;
}

void qqh_sort(void *input_arr, const size_t elem_cnt, const size_t elem_size, int (*comp)(const void *elem1, const void *elem2)) {
    assert(input_arr);
    assert(comp);

    char *arr = (char*) input_arr;

    for (size_t i = 0; i < elem_cnt; ++i) {
        for (size_t j = 0; j < elem_cnt - 1; ++j) {
            void *first = arr + j * elem_size;
            void *second = arr + (j + 1) * elem_size;
            if (comp(first, second) > 0) {
                swap_ptrs((void**) first, (void**) second);
            }
        }
    }
}

void UChar_get_next_symb(unsigned char **c) {
    assert(c);

    unsigned char *cur_c = *c;
    assert(cur_c != NULL);

    while(*cur_c && isspace(*cur_c)) {
        ++cur_c;
    }
    *c = cur_c;
}

void Char_get_next_symb(char **c) {
    assert(c);

    char *cur_c = *c;
    assert(cur_c != NULL);

    while(*cur_c && isspace(*cur_c)) {
        ++cur_c;
    }
    *c = cur_c;
}

char Char_in_string(const unsigned char c, const unsigned char *str) {
    while (*str) {
        if (c == *str) {
            return 1;
        }
        ++str;
    }
    return 0;
}

int compare_lines_letters(const void *elem1, const void *elem2) {
    unsigned char *first_c  = ((**(Line* *)elem1).string);
    unsigned char *second_c = ((**(Line* *)elem2).string);

    while (*first_c && *second_c) {
        UChar_get_next_symb(&first_c);
        UChar_get_next_symb(&second_c);

        if (*first_c != *second_c || (*first_c) * (*second_c) == 0) {
            return (int) *first_c - (int) *second_c;
        }

        ++first_c;
        ++second_c;
    }

    UChar_get_next_symb(&first_c);
    UChar_get_next_symb(&second_c);
    return (int) *first_c - (int) *second_c;
}

int reverse_compare_lines_letters(const void **elem1, const void **elem2) {
    return -compare_lines_letters(elem1, elem2);
}

void File_destruct(const File *file) {
    assert(file);

    Line **lines_ptr = file->lines;
    for (size_t i = 0; i < file->lines_cnt; ++i) {
        free(*lines_ptr);
        ++lines_ptr;
    }
    free(file->lines);
    free(file->text);
    fclose(file->ptr);
}

int File_construct(File *file, const char *name) {
    assert(file);
    assert(name);

    file->name = name;

    const int ret = read_lines(file);
    file->lines_cnt = (size_t) ret;
    file->cc = &file->text[0];
    return ret;
}


int read_lines(File *file) {
    assert(file);

    file->ptr = fopen(file->name, "rb");
    if (!file->ptr) {
        return -1;
    }

    fstat(fileno(file->ptr), &(file->info));

    file->text = (unsigned char*) calloc((size_t) file->info.st_size + 1, sizeof(char));
    if (!file->text) {
        return ERROR_MALLOC_FAIL;
    }

    fread(file->text, (size_t) file->info.st_size, 1, file->ptr);
    
    int lines_cnt = 0;
    for (unsigned char *c = file->text; *c; ++c) {
        lines_cnt += *c == '\n';
    }
    file->lines_cnt = (size_t) lines_cnt + 1;

    file->lines = (Line**) calloc((size_t) lines_cnt + 1, sizeof(Line*));
    if (file->lines == NULL) {
        return ERROR_MALLOC_FAIL;
    }

    unsigned char *c = file->text;
    lines_cnt = -1;
    int line_len = 0;
    int itrs = 0;
    while (itrs < file->info.st_size && *c) {
        ++lines_cnt;

        file->lines[lines_cnt] = (Line*) calloc(1, sizeof(Line));
        Line *line_ptr = file->lines[lines_cnt];
        if (line_ptr == NULL) {
            return ERROR_MALLOC_FAIL;
        }
        line_ptr->string = c;

        while(itrs < file->info.st_size && *c != '\n') {
            ++line_len;
            ++c;
            ++itrs;
            if (*c == '\r') {
                //*c = '\0';
            }
        }
        //*c = '\0';
        ++c;
        ++itrs;

        line_ptr->len = (size_t) line_len;
        line_len = 0;
    }

    return lines_cnt + 1;
}

char String_starts_with(const unsigned char *line, const char *sample) {
    if (!*line || !*sample) {
        return 0;
    }

    while (*line && *sample) {
        if (*line != *sample) {
            return 0;
        }
        ++line;
        ++sample;
    }

    return 1;
}

void print_file(const File *file, const char *fout_name, const char *mode) {
    assert(file);
    assert(fout_name);

    FILE *fout = fopen(fout_name, mode);
    for (size_t i = 0; i < file->lines_cnt; ++i) {
        fprintf(fout, "%s\n", file->lines[i]->string);
    }

    fclose(fout);
}

void print_error(int error) {
    if (error == 0) {
        return;
    }

    if (error == ERROR_FILE_NOT_FOUND) {
        printf("[ERR] File not found!\n");
    } else if (error == ERROR_BIG_FILE) {
        printf("[ERR] Can't handle such a big file!\n");
    } else if (error == ERROR_MALLOC_FAIL) {
        printf("[ERR] Can't allocate memory\n");
    } else {
        printf("[ERR](~!~)WERRORHUTGEERRORF(~!~)[ERR]\n");
    }
}
