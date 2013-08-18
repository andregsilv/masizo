#include <stdio.h>
#include <string.h>
void error_arguments() {
    puts("USAGE:\t masizo [--decompress] INFILE OUTFILE");
    exit(-1);
}
int main(int argc, char *argv[]) {
    if (argc > 2) {
        if (strcmp("--decompress",argv[1])==0) {
            if (argc > 3) {
                decompress(argv[2],argv[3]);
            } else error_arguments();
        } else {
            compress(argv[1],argv[2]);
        }
    } else error_arguments();
}
