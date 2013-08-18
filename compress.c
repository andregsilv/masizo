#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {
    long int len;
    char *chars;
} CharArray;
CharArray readToArray(FILE *file) {
    CharArray data;
    long int pos = ftell(file);
    fseek(file,0,SEEK_END);
    long int end = ftell(file);
    data.len = (end-pos) / sizeof(char);
    fseek(file,pos,SEEK_SET);
    data.chars = malloc(data.len * sizeof(char));
    for(int i=0; i<data.len; i++) {
        data.chars[i] = fgetc(file);
    }
    return data;
}
CharArray getTable(CharArray text) {
    CharArray table;
    table.len = 0;
    table.chars = malloc(sizeof(char) * 256);
    memset(table.chars,0,sizeof(char) * 256);
    for (int i=0; i<text.len; i++) {
        char contain = 0;
        for (int j=0; j<table.len; j++) {
            if (text.chars[i] == table.chars[j]) {
                contain = 1;
                break;
            }
        }
        if (!contain) {
            table.chars[table.len++] = text.chars[i];
        }
    }
    return table;
}
CharArray getRelativeArray(CharArray text, CharArray table) {
    CharArray relative;
    relative.chars = malloc(sizeof(char) * text.len);
    relative.len = text.len;
    for(int i=0; i<text.len; i++) {
        for (int j=0; j<table.len; j++) {
            if (text.chars[i] == table.chars[j]) {
                relative.chars[i] = j;
                break;
            }
        }
    }
    return relative;
}
void compress(char *file1, char *file2) {
    //allocate resources
    mpz_t data;
    mpz_init(data);
    FILE* inFile = fopen(file1,"r");
    FILE* outFile = fopen(file2,"w");
    //begin compression
    CharArray tx = readToArray(inFile);
    CharArray tab = getTable(tx);
    CharArray rel = getRelativeArray(tx,tab);
    //write size of table
    int base = tab.len;
    fputc(base-1,outFile);
    //write table
    for(int i=0; i<base; i++) {
        fputc(tab.chars[i],outFile);
    }
    putchar('\n');
    //adds to operator
    for(int i=rel.len-1; i>=0; i--) {
        mpz_mul_ui(data,data,base);
        mpz_add_ui(data,data,rel.chars[i]);
    }
    mpz_out_raw(outFile, data);
    //free resources
    free(rel.chars);
    free(tab.chars);
    free(tx.chars);
    fclose(outFile);
    fclose(inFile);
    mpz_clear(data);
}

void decompress(char *file1, char *file2) {
    //Allocate resources
    mpz_t data;
    mpz_init(data);
    FILE* inFile = fopen(file1,"r");
    FILE* outFile = fopen(file2,"w");
    //get size of table
    char base = fgetc(inFile) + 1;
    //read table
    char tab[base];
    for(int i=0; i<base; i++) {
        tab[i]= fgetc(inFile);
    }
    mpz_inp_raw(data, inFile);
    fclose(inFile);
    int c, i=0;
    while (mpz_cmp_ui(data,0)>0) { //Enquanto for maior que zero
        c = mpz_fdiv_ui(data,base);
        fputc(tab[c],outFile);
        mpz_sub_ui(data,data,c);
        mpz_div_ui(data,data,base);
    }
    //Release Resources
    fclose(outFile);
    mpz_clear(data);
}
