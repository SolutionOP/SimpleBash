#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define CountOfFiles 1024

typedef struct flg {
    int b;
    int e;
    int n;
    int s;
    int t;
    int v;
} flg_t;

int flg_parser(int argc, char *argv[], flg_t *flg);
int read_file(char *argv[], flg_t *flg);

int main(int argc, char *argv[]) {
    int err = 1;
    flg_t flg = {
        .b = 0,
        .e = 0,
        .n = 0,
        .s = 0,
        .t = 0,
        .v = 0,
    };
    err = flg_parser(argc, argv, &flg);
    if (flg.v) {
        flg.e = flg.t = 0;
    }
    if (flg.b == 1) {
        flg.n = 0;
    }

    while (optind < argc) {
        err = read_file(argv, &flg);
        optind++;
    }
    return err;
}

int flg_parser(int argc, char *argv[], flg_t *flg) {
    int c;
    int ret = 0;
    int index = 0;
    static struct option options[] = {
        {"number-nonblank", 0, 0, 'b'},
        {"number", 0, 0, 'n'},
        {"squeeze-blank", 0, 0, 's'},
        {0, 0, 0, 0},
    };

    while ((c = getopt_long(argc, argv, "bEnsTvet", options, &index)) != -1) {
        switch (c) {
        case 'b':
            flg->b = 1;
            break;

        case 'E':
            flg->e = 1;
            flg->v = 1;
            break;

        case 'n':
            flg->n = 1;
            break;

        case 's':
            flg->s = 1;
            break;

        case 'T':
            flg->t = 1;
            flg->v = 1;
            break;

        case 'v':
            flg->v = 1;
            break;

        case 'e':
            flg->e = 1;
            break;

        case 't':
            flg->t = 1;
            break;

        case '?':
            ret = -1;
            break;
        }
    }
    return ret;
}

int read_file(char *argv[], flg_t *flg) {
    FILE *file;
    int ret = 0;

    file = fopen(argv[optind], "r");

    if (file != NULL) {
        int str_count = 1;
        int empty_str_count = 0;
        int last_sym = '\n';

        while (1) {
            int cur_c = fgetc(file);
            if (cur_c == EOF) {
                break;
            }
            if (flg->s && cur_c == '\n' && last_sym == '\n') {
                empty_str_count++;
                if (empty_str_count > 1) {
                    continue;
                }
            } else {
                empty_str_count = 0;
            }
            if (last_sym == '\n' && ((flg->b && cur_c != '\n') || flg->n)) {
                printf("%6d\t", str_count++);
            }
            if (flg->t && cur_c == '\t') {
                printf("^");
                cur_c = 'I';
            }
            if (flg->e && cur_c == '\n') {
                printf("$");
            }
            if (flg->v) {
                if ((cur_c >= 0 && cur_c < 9) || (cur_c > 10 && cur_c < 32)) {
                printf("^");
                cur_c += 64;
                }
            }
            printf("%c", cur_c);
            last_sym = cur_c;
        }
        fclose(file);
    } else {
        ret = -1;
    }
    return ret;
}
