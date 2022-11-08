#include <stdio.h>
#include <math.h>
#include <strings.h>
#include "list.h"

int main()
{

    char* graphlog = (char*) "out\\graphlog.html";

    struct List list = {};

    ListCtor(&list, 10, graphlog);

    GraphDump(&list);

    int pos2 = InsertBeforeIndex(&list, 40, list.data[0].next);

    GraphDump(&list);

    int pos3 = InsertBeforeIndex(&list, 80, pos2);

    GraphDump(&list);

    int pos4 = InsertBeforeIndex(&list, 35, list.data[pos3].next);

    GraphDump(&list);

    InsertBeforeIndex(&list, 25, pos4);

    GraphDump(&list);

    DeleteElement(&list, pos4);

    GraphDump(&list);

    DeleteElement(&list, pos2);

    GraphDump(&list);

    ListSort(&list);

    GraphDump(&list);

    InsertBeforeIndex(&list, 30, list.data[0].next);

    GraphDump(&list);

    ListDtor(&list);

    return 0;
}