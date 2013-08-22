#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    long int len;
    unsigned char *chars;
} CharArray;
CharArray readToArray(FILE *file) {
    CharArray data;
    long int pos = ftell(file);
    fseek(file,0,SEEK_END);
    long int end = ftell(file);
    data.len = end-pos;
    fseek(file,pos,SEEK_SET);
    data.chars = malloc(data.len);
    for(int i=0; i<data.len; i++) {
        data.chars[i] = fgetc(file);
    }
    return data;
}
CharArray getTable(CharArray text) {
    CharArray table;
    table.len = 0;
    table.chars = malloc(256);
    memset(table.chars,0,256);
    for (int i=0; i<text.len; i++) {
        char contains = 0;
        for (int j=0; j<table.len; j++) {
            if (text.chars[i] == table.chars[j]) {
                contains = 1;
                break;
            }
        }
        if (!contains) {
            table.chars[table.len++] = text.chars[i];
        }
    }
    return table;
}
CharArray getRelativeArray(CharArray text, CharArray table) {
    CharArray relative;
    relative.chars = malloc(text.len);
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
    FILE *inFile = fopen(file1,"rb");
    //begin compression
    CharArray tx = readToArray(inFile);
    fclose(inFile);

    CharArray tab = getTable(tx);
    CharArray rel = getRelativeArray(tx,tab);
    free(tx.chars);

    FILE *outFile = fopen(file2,"wb");
    //write size of table
    int base = tab.len;
    printf("Base is %i\n",base);
    fputc(base-1,outFile);
    //write table
    for(int i=0; i<base; i++) {
        fputc(tab.chars[i],outFile);
    }
    //adds to operator
    for(int i=rel.len-1; i>=0; i--) {
        mpz_mul_ui(data,data,base);
        mpz_add_ui(data,data,rel.chars[i]);
    }
    FILE *log = fopen("log1","wb");
	  gmp_fprintf(log,"%Zd\n",data);
    fclose(log);

    size_t a = mpz_out_raw(outFile, data);
    printf("%d bytes written\n",a);
    //free resources
    free(rel.chars);
    free(tab.chars);
    fclose(outFile);
    mpz_clear(data);
}

void decompress(char *file1, char *file2) {
    //Allocate resources
    mpz_t data;
    mpz_init(data);
    FILE *inFile = fopen(file1,"rb");
    //get size of table
    int base = fgetc(inFile) + 1;
    printf("Base is %i\n",base);
    //read table
    char tab[base];
    for(int i=0; i<base; i++) {
        tab[i]= fgetc(inFile);
    }
    size_t a = mpz_inp_raw(data, inFile);
    printf("%d bytes read\n",a);
    fclose(inFile);

    FILE *log = fopen("log2","wb");
	  gmp_fprintf(log,"%Zd\n",data);
    fclose(log);

    int i=0;
    unsigned long int remainder;
    FILE *outFile = fopen(file2,"wb");
    while (mpz_cmp_ui(data,0)>0) { //Enquanto for maior que zero
        remainder = mpz_fdiv_q_ui(data,data,base);
        fputc(tab[remainder],outFile);
    }
    //Release Resources
    fclose(outFile);
    mpz_clear(data);
}
