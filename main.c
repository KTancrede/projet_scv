#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "hash.h"
#include "lcc.h"
#include "git.h"
#include "wf.h"
#include "commit.h"

int main(){
    Commit *c=ftc("t.txt");
    char*str=cts(c);
    printf("%s\n",str);
}