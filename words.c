#include "words.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

void words_start(word_t **wordlist, int size)
{

    word_t *tmp = NULL;

    tmp = calloc(size+1, sizeof(word_t));
    assert(tmp != NULL);
    *wordlist = tmp;

}

static
int s_word_count_lines(FILE *file){

    int ch = 0;
    int lines = 0;
    do {
        ch = fgetc(file);

        if(ch == '\n'){
            lines++;
        }

    }while(ch != EOF);

    return lines;
}

int words_load_file(wordlist_t *wordlist, const char *filepath)
{
    FILE * fp = NULL;

    if((fp = fopen(filepath,"rb")) == NULL){
        return -1;
    }

    char line[255]  = {0};
    int line_count = 0;

    line_count = s_word_count_lines(fp);
    wordlist->total_words = line_count;
    words_start(&wordlist->words, line_count);
    fseek(fp,0, SEEK_SET);


    word_t *words_tmp = wordlist->words;

    for(int i = 0; i < line_count;i++){

        if(fgets(line,255,fp) != NULL){

            int size = strlen(line);
            strncpy(words_tmp[i].word, line, size);

            for(int k = 0; k <words_tmp[k].len;k++){
                words_tmp[i].word[k] = tolower(words_tmp[i].word[k]);
            }

            words_tmp[i].len = size-1;
            words_tmp[i].hit = 0;
        }


    }


    return 0;
}

wordlist_t *wordlist_sort(const wordlist_t *list, int size)
{
    //word_t *tmp = NULL;
    //tmp = calloc(size, sizeof(word_t));

    if(size > list->total_words)
        size = list->total_words;

    wordlist_t *list_tmp = NULL;

    list_tmp = malloc(sizeof(wordlist_t));
    list_tmp->total_words = 0;
    list_tmp->words = NULL;
    list_tmp->total_words = size;

    words_start(&list_tmp->words, size);


    for(int i = 0; i < size;i++){
        int rnd = rand() % list->total_words - 1;

        word_t *word = &list->words[rnd];
        memcpy(&list_tmp->words[i], word, sizeof(word_t));
    }





    return list_tmp;
}

void wordlist_unset(wordlist_t *wordlist)
{
    if(wordlist){
        if(wordlist->words){
            free(wordlist->words);
            wordlist->words  = NULL;
        }

        free(wordlist);
        wordlist = NULL;
    }
}
