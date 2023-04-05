#define MAX_FILES 200
#define N 200
typedef struct cell {
    char* data;
    struct cell* next;
} Cell;

typedef Cell* List;

List* initList();
Cell* buildCell(char* ch);
void insertFirst(List *L, Cell* C);
char* ctos(Cell* c);
char* ltos(List* L);
Cell* listGet(List* L, int i);
Cell* searchList(List* L, char* str);
List* stol(char* s);
void ltof(List* L, char* path);
List* ftol(char* path);
