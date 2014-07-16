
#include <stdio.h>
#include "table.h"
#include "dlist.h"
#include "array.h"
#define element10k 10000


typedef struct MyTable {
    array *arr;
    CompareFunction *cf;
    KeyFreeFunc *keyFree;
    ValueFreeFunc *valueFree;
    // La till denna för att spåra sista elementet
    int LastElementIndex;
} MyTable;

typedef struct TableElement{
    KEY key;
    VALUE value;
} TableElement;

/* Creates a table.
 *  compare_function - Pointer to a function that is called for comparing
 *                     two keys. The function should return <0 if the left
 *                     parameter is smaller than the right parameter, 0 if
 *                     the parameters are equal, and >0 if the left
 *                     parameter is larger than the right item.
 * Returns: A pointer to the table. NULL if creation of the table failed. */
Table *table_create(CompareFunction *compare_function)
{
    MyTable *t = calloc(sizeof (MyTable),1);
    if (!t)
    {
        return NULL;
    }
    t->arr=array_create(1,0,element10k);
    array_setMemHandler(t->arr, free);
    t->cf = compare_function;
    return t;
}

/*
 *  freeFunc- Pointer to a function that is called for  freeing all
 *                     the memory used by keys inserted into the table*/
void table_setKeyMemHandler(Table *table,KeyFreeFunc *freeFunc) {
    MyTable *t = (MyTable*)table;
    t->keyFree=t->arr->freeFunc;
}
/*
 *  freeFunc- Pointer to a function that is called for  freeing all
 *                     the memory used by keys inserted into the table*/
void table_setValueMemHandler(Table *table,ValueFreeFunc *freeFunc) {
    MyTable *t = (MyTable*)table;
    t->valueFree=t->arr->freeFunc;
}

/* Determines if the table is empty.
 *  table - Pointer to the table.
 * Returns: false if the table is not empty, true if it is. */
bool table_isEmpty(Table *table) {
    MyTable *t = (MyTable*)table;
    return (array_hasValue(t->arr,0)==0);
}

/*This function looks up the element Corresponding to the given key*/
VALUE table_lookup(Table *table, KEY key) {
    TableElement *i;
    MyTable *t = (MyTable*)table;
    int k=0;
    while(array_hasValue(t->arr, k) &&  k < element10k )
    {
        i=array_inspectValue(t->arr,k);
        if(t->cf(i->key,key)==0)
        {
            return i->value;
        }
        k++;
    }
    return NULL;
}

/*This function removes the table */
void table_free(Table *table) {
    MyTable *t = (MyTable*)table;
    array_free(t->arr);
    free(t);
}



/* Inserts a key and value pair into the table. The table type takes
 * ownership of the key and value pointers and is responsible for calling
 * the destroy function to free them when they are removed.
 *  table - Pointer to the table.
 *  key   - Pointer to the key.
 *  value - Pointer to the value.
 */
void table_insert(Table *table, KEY key,VALUE value) {
    
    MyTable *t = (MyTable*)table;
    TableElement *e=malloc(sizeof(TableElement));
    TableElement *p;
    e->key = key;
    e->value = value;
    int i=0;
    p=array_inspectValue(t->arr,i);
    while(array_hasValue(t->arr,i) && t->cf(key,p->key)!=0)
    {
        i++;
        array_inspectValue(t->arr,i);
    }
    if(i<element10k)
    {
        array_setValue(t->arr,e,i);
        t->LastElementIndex = i;
    }

}



/* This function removes the element Corresponding to the given key*/

// ***>>>> AND sets the last element on this index.
void table_remove(Table *table, KEY key) {

        MyTable *t = (MyTable*)table;
        TableElement *i;
        TableElement *last;
        TableElement *temp;
        int k = 0;
        while (k < element10k && array_hasValue(t->arr, k))
        {
            i = (TableElement*)array_inspectValue(t->arr, k);
            if (t->cf(i->key,key)==0)
            {
                    if(t->keyFree!=NULL)
                    {
                        t->keyFree(i->key);
                    }
                    if(t->valueFree!=NULL)
                    {
                        t->valueFree(i->value);
                    }

                    last = array_inspectValue(t->arr , t->LastElementIndex);
                    temp = malloc(sizeof(TableElement));
                    temp->key = last->key;
                    temp->value = last->value;

                    array_setValue(t->arr, temp , k);
                    array_setValue(t->arr, NULL , t->LastElementIndex);

                    t->LastElementIndex--;
                    break;
            }
            k++;
        }
}

