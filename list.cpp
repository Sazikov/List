#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "list.h"

int Piccounter = 1;

#define LISTCHECK(List_name)   do  {                                                                                \
                                        ListVerificator(List_name);                                                 \
                                        TextDump(List_name, __LINE__, __func__, __FILE__);                          \
                                    } while(0);

#define SIZECHECK   do  {                                                                                   \
                            if (list->size == list->capacity - 1)                                           \
                                ListResize(list, ENLARGE);                                                  \
                            else if (list->size < list->capacity * SHRINKCOEFF && list->status == SORTED)   \
                                ListResize(list, SHRINK);                                                   \
                        } while(0);

int ListCtor(struct List* list, const size_t capacity, char* graphlog)
{
    list->data = (struct ElemList*) calloc(capacity + 1, sizeof(struct ElemList));
    if ((list->data) == NULL)
        return MEMERROR;

    list->data[0].elem = POISON;
    list->data[capacity].next = 0;

    Poison(list, 1, capacity + 1);

    list->free = 1;
    list->data[0].next = 0;
    list->data[0].prev = 0;
    list->size = 0;
    list->capacity = capacity;
    list->errors = 0;
    list->status = SORTED;

    list->graphlog = graphlog;

    LISTCHECK(list)

    return NOERROR;
}

int ListDtor(struct List* list)
{
    assert(list != NULL);

    LISTCHECK(list)

    free(list->data);
    list->data = NULL;

    list->free = -1;
    list->data[0].next = -1;
    list->data[0].prev = -1;
    list->size = -1;

    return NOERROR;
}

int Poison(struct List* list, const int start, const int end)
{
    assert(list != NULL);
    
    int temp = list->size + 1;

    for (int i = start; i < end; i++)
    {
        list->data[i].elem = POISON;
        list->data[i].prev = -1;
        list->data[i].next = temp + 1;
        temp++;
    }

    return NOERROR;
}

void ListVerificator(struct List* list)
{
    assert(list != NULL);

    if (list->free <= 0)
        list->errors |= FREEERROR;
    if (list->data == NULL)
        list->errors |= DATAERROR;
    if (list->data[0].next < 0)
        list->errors |= HEADERROR;
    if (list->size > list->capacity)
       list->errors |= SIZEERROR;
    if (list->data[0].prev < 0)
        list->errors |= TAILERROR;

    int index = list->data[0].next;
    if (list->size > 1)
    {
        if (list->data[list->data[0].next].prev != 0 || list->data[list->data[list->data[0].next].next].prev != list->data[0].next)
        {
            list->errors |= HEADERROR;
        }

        if (list->data[list->data[0].prev].next != 0 || list->data[list->data[list->data[0].prev].prev].next != list->data[0].prev)
            list->errors |= TAILERROR;
        while (index != list->data[list->data[0].prev].prev)
        {
            index = list->data[index].next;
            if (list->data[list->data[index].next].prev != index || list->data[list->data[index].prev].next != index)
            {
                list->errors |= ELEMERROR;
            }
        }
    }
    else if (list->size == 1)
    {
        if (list->data[list->data[0].next].prev != 0 || list->data[list->data[0].prev].next != 0)
            list->errors |= ELEMERROR;
    }

    index = list->free;
    while ((size_t) index <= list->capacity)
    {
        if (list->data[index].next <= 0)
        {
            list->errors |= FREEERROR;
            continue;
        }

        index = list->data[index].next;
    }
}

int TextDump(const struct List* list, const int line, const char* func, const char* file)
{
    assert(list != NULL);

    FILE* logs = fopen("out\\logs.txt", "a");

    fprintf(logs, "\n\n%s at ", func);
    fprintf(logs, "%s", file);
    fprintf(logs, "(%d)\n", line);
    fprintf(logs, "List[%p] ", (void*) list);
    if (list->errors == 0)
        fprintf(logs, "(OK)\n");
    else
        fprintf(logs, "(ERROR CODE = %d)\n", list->errors);

    fprintf(logs, "head = %d\n", list->data[0].next);
    fprintf(logs, "tail = %d\n", list->data[0].prev);

    fprintf(logs, "size = %lld\n", list->size);
    fprintf(logs, "capacity = %lld\n", list->capacity);
    fprintf(logs, "free = %d\n", list->free);
    if (list->status == SORTED)
        fprintf(logs, "status = SORTED\n");
    else
        fprintf(logs, "status = UNSORTED\n");

    fprintf(logs, "index:");
    for (size_t i = 1; i < list->capacity + 1; i++)
        fprintf(logs, "%2lld ", i);

    fprintf(logs, "\n%6s", "val:");
    for (size_t i = 1; i < list->capacity + 1; i++)
        if (isnan(list->data[i].elem))
            fprintf(logs, "PS ");
        else
            fprintf(logs, "%2.0lf ", list->data[i].elem);

    fprintf(logs, "\n%6s", "next:");
    for (size_t i = 1; i < list->capacity + 1; i++)
        fprintf(logs, "%2d ", list->data[i].next);
        
    fprintf(logs, "\n%6s", "prev:");
    for (size_t i = 1; i < list->capacity + 1; i++)
        fprintf(logs, "%2d ", list->data[i].prev);

    fclose(logs);

    return NOERROR;
}

int GraphDump(const struct List* list)
{
    assert(list != NULL);

    char picture[100] = "picture";
    sprintf(picture, "out\\%d.dot", Piccounter);
    char picturesrc[100] = "picture";
    sprintf(picturesrc, "out\\%d.png", Piccounter);

    FILE* pic = fopen(picture, "w");

    fprintf(pic, "DiGraph List {\n");
    fprintf(pic, "\trankdir = TB\n");

    fprintf(pic, "\t\"box%d\" [shape = \"record\", style = \"filled\", fillcolor = \"pink2\", label = \"{index = %d|PSN|prev = %d|next = %d}\"]\n", 0, 0, list->data[0].prev, list->data[0].next);
    for (size_t i = 1; i < list->capacity + 1; i++)
    {
        if (isnan(list->data[i].elem))
            fprintf(pic, "\t\"box%lld\" [shape = \"record\", style = \"filled\", fillcolor = \"lightgrey\", label = \"{index = %lld|PSN|prev = %d|next = %d}\"]\n", i, i, list->data[i].prev, list->data[i].next);
        else
            fprintf(pic, "\t\"box%lld\" [shape = \"record\", style = \"filled\", fillcolor = \"yellow\", label = \"{index = %lld|val = %.0lf|prev = %d|next = %d}\"]\n", i, i, list->data[i].elem, list->data[i].prev, list->data[i].next);
    }
    fprintf(pic, "\t\"Tail\" [shape = \"record\", style = \"filled\", fillcolor = \"red\", label = \"Tail\"]\n");
    fprintf(pic, "\t\"Head\" [shape = \"record\", style = \"filled\", fillcolor = \"orange\", label = \"Head\"]\n");
    fprintf(pic, "\t\"Free\" [shape = \"record\", style = \"filled\", fillcolor = \"green\", label = \"Free\"]\n");

    fprintf(pic, "\t{rank = same;");
    for (size_t i = 0; i < list->capacity + 1; i++)
        fprintf(pic, "box%lld; ", i);
    fprintf(pic, "}\n");

    for (size_t i = 0; i < list->capacity; i++)
    {
        fprintf(pic, "\t\"box%lld\" -> \"box%lld\"[style = \"invis\"];\n", i, i + 1);
    }

    for (size_t i = 0; i < list->capacity + 1; i++)
    {
        if (list->data[i].next != -1 && (size_t) list->data[i].next < list->capacity + 1)
            fprintf(pic, "\t\"box%lld\" -> \"box%d\";\n", i, list->data[i].next);
        if (list->data[i].prev != -1)
            fprintf(pic, "\t\"box%lld\" -> \"box%d\";\n", i, list->data[i].prev);
    }

    fprintf(pic, "\t\"Tail\" -> \"box%d\";\n", list->data[0].prev);
    fprintf(pic, "\t\"Head\" -> \"box%d\";\n", list->data[0].next);
    fprintf(pic, "\t\"Free\" -> \"box%d\";\n", list->free);

    fprintf(pic, "}");

    fclose(pic);
    Piccounter += 1;

    char src[500] = "";
    sprintf(src, "dot -Tpng %s -o %s", picture, picturesrc);
    
    system(src);

    FILE* graphlog = fopen(list->graphlog, "a");

    fprintf(graphlog, "<pre>\n");

    fprintf(graphlog, "<img src = \"..\\%s\">\n", picturesrc);

    fprintf(graphlog, "<hr>\n");

    fclose(graphlog);

    return NOERROR;
}

int InsertBeforeIndex(struct List* list, const elem_t val, const int index)
{
    assert(list != NULL);
    LISTCHECK(list)

    int newelem = list->free;
    list->free = list->data[newelem].next;

    list->data[newelem].elem = val;
    list->data[newelem].prev = list->data[index].prev;
    list->data[newelem].next = index;

    if(list->data[list->data[index].prev].next != -1)
        list->data[list->data[index].prev].next = newelem;
    list->data[index].prev = newelem;
    
    if (index == list->data[0].next)
    {
        list->data[0].next = newelem;
        list->data[newelem].prev = 0;
    }
    list->size++;
    list->status = UNSORTED;

    SIZECHECK
    LISTCHECK(list)

    return newelem;
}

int ListResize(struct List* list, const int mode)
{
    assert(list != NULL);
    LISTCHECK(list)

    if (mode == ENLARGE)
        list->capacity *= RESIZECOEFF;
    else if (mode == SHRINK && list->status == SORTED && list->size * 2 < list->capacity)
        list->capacity /= RESIZECOEFF;
    else
        return RESIZERROR;

    struct ElemList* prev = list->data;

    struct ElemList* buffer = (struct ElemList*) realloc(list->data, (list->capacity + 1) * sizeof(struct ElemList));
    if (buffer == NULL)
        return MEMERROR;

    list->data = buffer;

    Poison(list, list->size + 1, list->capacity + 1);

    if (prev != list->data)
        free(prev);

    return NOERROR;
}

int DeleteElement(struct List* list, const int index)
{
    assert(list != NULL);
    LISTCHECK(list)

    list->data[index].elem = 0;

    list->data[list->data[index].next].prev = list->data[index].prev;
    list->data[list->data[index].prev].next = list->data[index].next;

    if (index == list->data[0].next)
        list->data[0].next = list->data[index].next;

    if (index == list->data[0].prev)
        list->data[0].prev = list->data[index].prev;

    list->data[index].elem = POISON;
    list->data[index].next = list->free;
    list->data[index].prev = -1;

    list->free = index;
    list->size -= 1;
    list->status = UNSORTED;

    LISTCHECK(list)

    return index;
}

int FindElemByVal(struct List* list, const elem_t val)
{
    assert(list != NULL);
    LISTCHECK(list)

    int index = list->data[0].next;
    while (index != list->data[0].prev)
    {
        if (compare(list->data[index].elem, val) == 0)
            return index;
        index = list->data[index].next;
    }

    return -1;
}

int FindElemByLogicIndex(struct List* list, const int logicindex)
{
    assert(list != NULL);
    LISTCHECK(list)

    if (list->status == SORTED && (size_t) logicindex <= list->size)
        return logicindex;

    else if ((size_t) logicindex > list->size)
        return SIZEERROR;

    int counter = 0;
    int index = list->data[0].next;

    while (counter < logicindex)
    {
        index = list->data[index].next;
        counter++;
    }

    return index;
}

int compare(const elem_t a, const elem_t b)
{
    assert(isfinite(a));
    assert(isfinite(b));

    if (fabs(a-b) < EPSILON)
        return EQUAL;
    if ((a-b) > EPSILON)
        return MORE;
    if ((a-b) < EPSILON)
        return LESS;

    return NOERROR;
}

int ListSort(struct List* list)
{
    assert(list != 0);
    LISTCHECK(list)

    struct ElemList* newdata = (struct ElemList*) calloc(list->capacity + 1, sizeof(struct ElemList));
    if (newdata == NULL)
        return MEMERROR;

    newdata[0].elem = POISON;

    int index = list->data[0].next;
    size_t counter = 1;

    while (counter <= list->size)
    {
        newdata[counter].elem = list->data[index].elem;
        newdata[counter].next = counter + 1;
        newdata[counter].prev = counter - 1;

        counter++;
        index = list->data[index].next;
    }

    free(list->data);

    list->data = newdata;
    list->data[0].prev = counter - 1;
    list->data[0].next = 1;
    list->data[list->data[0].prev].next = 0;
    list->data[list->data[0].next].prev = 0;
    list->free = counter;
    list->status = SORTED;

    Poison(list, list->free, list->capacity + 1);

    SIZECHECK
    LISTCHECK(list)

    return NOERROR;
}

