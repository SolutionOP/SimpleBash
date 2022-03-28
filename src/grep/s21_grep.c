#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <getopt.h>
#define buff_size 999999

struct flags {
    int is_e;
    int is_i;
    int is_v;
    int is_c;
    int is_l;
    int is_n;
    int is_h;
    int is_s;
    int is_f;
    int is_o;
};

int Parsing(int argc, char *argv[], struct flags *_flags, char *pattern);
void filework(struct flags *_flags, FILE *fp, regex_t reg, char *file);
void grep_file(struct flags *_flags, char *pattern, char *file);
int save_patt_file(char *pattern, char *temp);
void grep_main(struct flags *_flags, int argc, char **argv, char *temp);


int main(int argc, char *argv[]) {
    struct flags _flags;
    int returnResult = 0;
    char pattern[buff_size] = {0};
    memset(&_flags, 0, sizeof(_flags));
    returnResult = Parsing(argc, argv, &_flags, pattern);
    if ((argc >= 3) && (returnResult != 1)) {
        grep_main(&_flags, argc, argv, pattern);
    }
    return returnResult;
}


int Parsing(int argc, char *argv[], struct flags *_flags, char *pattern) {
    int i = 0, result = 0;
    while ((i = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
        if (i == 'e') {
        _flags->is_e = 1;
        snprintf(pattern, buff_size, optarg);
        } else if (i == 'i') {
        _flags->is_i = 1;
        } else if (i == 'v') {
        _flags->is_v = 1;
        } else if (i == 'c') {
        _flags->is_c = 1;
        } else if (i == 'l') {
        _flags->is_l = 1;
        } else if (i == 'n') {
        _flags->is_n = 1;
        } else if (i == 'h') {
        _flags->is_h = 1;
        } else if (i == 's') {
        _flags->is_s = 1;
        } else if (i == 'f') {
        _flags->is_f = 1;
        snprintf(pattern, buff_size, optarg);
        } else if (i == 'o') {
        _flags->is_o = 1;
        } else {
        result = 1;
        }
    }
    return result;
}

void grep_main(struct flags *_flags, int argc, char **argv, char *temp) {
  char patternL[buff_size] = {0};
  int cols = 0;

  if (!_flags->is_f && !_flags->is_e) snprintf(patternL, buff_size, argv[optind++]);
  if (_flags->is_f) cols = save_patt_file(patternL, temp);
  if (!_flags->is_f && _flags->is_e) snprintf(patternL, buff_size, temp);

  if (cols != -1) {
    int file_c = (argc - optind > 1) ? 1 : 0;

    for (int i = optind; i < argc; i++) {
      if (file_c && !_flags->is_h && !_flags->is_l) printf("%s:", argv[i]);
      grep_file(_flags, patternL, argv[i]);
    }
  }
}

int save_patt_file(char *pattern, char *temp) {
  FILE *fp;
  fp = fopen(temp, "r");
  int i = 0;

  if (fp == NULL) {
    i = -1;
  } else {
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
      if (ch == 13 || ch == 10) pattern[i++] = '|';
      if (ch != 13 && ch != 10) pattern[i++] = ch;
    }

    if (pattern[i - 1] == '|') pattern[i - 1] = '\0';
    fclose(fp);
  }
  return (i == -1) ? -1 : (i + 1);
}

void grep_file(struct flags *_flags, char *pattern, char *file) {
  int cflags = (_flags->is_i) ? REG_ICASE | REG_EXTENDED : REG_EXTENDED;
  regex_t reg;
  FILE *fp;
  fp = fopen(file, "r");

  if (fp == NULL) {
  } else {
    regcomp(&reg, pattern, cflags);
    filework(_flags, fp, reg, file);
    regfree(&reg);
    fclose(fp);
  }
}

void filework(struct flags *_flags, FILE *fp, regex_t reg, char *file) {
  char text[buff_size] = {0};
  regmatch_t pmatch[1];
  int line_matches = 0, nline = 1;

  while (fgets(text, buff_size - 1, fp) != NULL) {
    int status = regexec(&reg, text, 1, pmatch, 0);
    int match = 0;

    if (status == 0 && !_flags->is_v) match = 1;
    if (status == REG_NOMATCH && _flags->is_v) match = 1;
    if (match && !_flags->is_l && !_flags->is_c && _flags->is_n) printf("%d:", nline);
    if (match && !_flags->is_l && !_flags->is_c && !_flags->is_o) printf("%s", text);

    if (_flags->is_o && match) {
      for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
        printf("%c", text[i]);
      }
      printf("\n");
    }
    line_matches += match;
    nline++;
  }

  if (_flags->is_l && line_matches > 0) printf("%s\n", file);
  if (_flags->is_c && !_flags->is_l) printf("%d\n", line_matches);
}
