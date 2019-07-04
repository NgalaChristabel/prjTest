#include <stdio.h>
#include "listADT.c"
#include "listOfRelationsADT.c"


//define a record data structure for a frame(record data structure)
struct frame{
    handy_list object;
    union relations R;
};

int main()
{
    struct frame frm;

    init(&frm.R);

    list = handy_create_list();

    int item;


    printf("Enter int: ");
    scanf("%d", &item);

    frm.R.ref(item );

    checkRelation1( item );

    printf("Enter int: ");
    scanf("%d", &item);

    checkRelation1( item );

    printf("list: %d", list->get_front(list) );
    printf("list: %d", list->get_back(list) );

}





