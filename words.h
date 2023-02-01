#ifndef WORDS_H
#define WORDS_H


typedef struct word_t {
    char word[255];
    int  len;
    int hit;
    float x;
    float y;
}word_t;


typedef struct wordlist_t {
    word_t *words;
    int total_words;
}wordlist_t;

void words_start(word_t **wordlist, int size);
int words_load_file(wordlist_t *wordlist, const char *filepath);

wordlist_t *wordlist_sort(const wordlist_t *list, int size);


void wordlist_unset(wordlist_t *wordlist);

#endif // WORDS_H
