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
    kvp *kk=stkv("ad: as");
    printf("%s\n",kvts(kk));
    freeKeyVal(kk);
    
}