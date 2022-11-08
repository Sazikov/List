#ifndef LIST_H
#define LIST_H

typedef double elem_t;

const double POISON = NAN;
const double RESIZECOEFF = 2.0;
const double SHRINKCOEFF = 0.375;
const double EPSILON = 1e-10;

struct ElemList
{
    elem_t elem;
    int next;
    int prev;
};

enum Status
{
    UNSORTED = 0,
    SORTED   = 1,
};

struct List
{
    struct ElemList* data;

    int free;

    size_t size;
    size_t capacity;

    char* graphlog;
    int errors;
    int status;
};

enum Compare_result
{
    MORE  =  1,
    EQUAL =  0,
    LESS  = -1,
};

int ListCtor(struct List* list, const size_t capacity, char* graphlog);

int InsertBeforeIndex(struct List* list, const elem_t val, const int index);

int ListResize(struct List* list, const int mode);

int Poison(struct List* list, const int start, const int end);

int DeleteElement(struct List* list, const int index);

int FindElemByVal(struct List* list, const elem_t val);

int FindElemByLogicIndex(struct List* list, const int index);

int compare(const elem_t a, const elem_t b);

int ListSort(struct List* list);

void ListVerificator(struct List* list);

int TextDump(const struct List* list, const int line, const char* func, const char* file);

int GraphDump(const struct List* list);

int ListDtor(struct List* list);

enum SysErrors
{
    NOERROR   = 0,
    MEMERROR  = 1,
};

enum ListErrors
{
    ELEMERROR  = 2,
    TAILERROR  = 4,
    HEADERROR  = 8,
    SIZEERROR  = 16,
    FREEERROR  = 32,
    DATAERROR  = 64,
    RESIZERROR = 128,
};

enum ResizeModes
{
    ENLARGE  = 1,
    SHRINK   = 2,
};

#endif //LIST_H