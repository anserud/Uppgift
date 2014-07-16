/* Written by Johan Eliasson <johane@cs.umu.se>.
May be used in the course Datastrukturer och Algoritmer (Python) at UmeÂ University.
Usage exept those listed above requires permission by the author.
*/


/* Correctness testing algorithm:
 * 1.  Create a table.
 * 2.  Check isempty = True.
 * 3.  Attempt to lookup a nonexistent item.
 * 4   Insert an item.
 * 5   Check isempty = False.
 * 6   Lookup the item, check value.
 * 7   Attempt to lookup a nonexistent item.
 * 8   Attempt to remove a nonexistent item.
 * 9   Check isempty = False.
 * 10  Remove the inserted item.
 * 11  Check isempty = True.
 * 12. For N = 10% av tabellstorlek, 65% av tabellstorlek och tabellstorlek:
 * 12.1   Generate N*2 random pairs with unique keys.
 * 12.2   Insert half of the generated pairs.
 * 12.3   Check isempty = False
 * 12.4   Lookup all the generated pairs (half of them should fail).
 * 12.5   Remove half of the previously inserted pairs.
 * 12.6   Check isempty = False
 * 12.7   Lookup all the pairs that were inserted at first (half of them
 *       should fail).
 * 12.8   Check isempty = False
 * 12.9   Remove all the remaining pairs.
 * 12.10  Check isempty = True
 * 13  Check that the table handles duplicates correctly
 *
 * There is also a module measuring time for insertions, lookups etc.
 * */
#include "table.h"
#include "array.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Size of the table to generate
#define TABLESIZE 10000
// Number of accesses to make to the table during lookup speed tests
#define NROFACCESSES TABLESIZE*2
// The size of the random object space to generate
#define RANDOMRANGE TABLESIZE*3

/*Helper function to allocate memory for and initialize a integer pointer
 the value pointed to will be i */
int *intPtrFromInt(int i){
    int *ip=malloc(sizeof(int));
    *ip=i;
    return ip;
}

int indent=0;

/* Function to get time in ms */
unsigned long get_milliseconds()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (unsigned long)(tv.tv_sec*1000 + tv.tv_usec/1000);
}

/*Compare function used to compare two int values (pointed to by ip and ip2 ar equal*/
int compareInt(void *ip,void *ip2){
    return (*(int*)ip) - (*(int*)ip2);
}

/*
 Used for pretty printing, handles the indentation of text
 */
void indenting() {

    indent = indent + 2;
}

/*
 Used for pretty printing, handles the outdentation of text
 */
void outdenting() {

    if(indent>=2)
        indent = indent-2;
}

/*
 Used for pretty printing, handles the printing of text regarding tests
 msg - the message to be printed
 */
void test(char *msg) {

    printf("%*s%s: \n",indent,"", msg);
}

/*
 Used for pretty printing, handles the printing of text regarding tests
 msg - the message to be printed
 */
void comprehensiveTest(char *msg) {

    printf("%*s%s...\n",indent,"", msg);
}

/*
 Checks a test and prints OK of it works and FAILED otherwize
 success - a test to be checked (must give a boolean result)
 */
void check(bool success) {

    if(success)
        printf("OK\n");
    else {
        printf("FAILED\n");
        exit(1);
    }
}

/*
 Checks a test and prints FAILED plus a message if it does not work then exits the application
 success - a test to be checked (must give a boolean result)
 message - the text to be written if it does not work
 */
void silentCheck(bool success,char *message) {

    if(!success) {
        printf("FAILED %s\n", message);
        exit(1);
    }
}

/*
 Makes a lookup-test of a list of keys and values
 keys - a list of keys to be tested
 values - a list of values to be tested
 inserted - a list of booleans that tells if the key and value at
 index i was inserted in the table or not
 items - the length of the supplied lists
 table - the table to do the lookups in
 */
void testLookups(int keys[],int values[], bool inserted[],int items,Table *table) {

    for(int i=0;i<items;i++) {
        int *value = table_lookup(table,&keys[i]);
        if(inserted[i]){
            silentCheck(value!= NULL, "key not found");
            silentCheck(*value==values[i], "looked-up value is incorrect");
        }
        else {
            silentCheck(value==NULL, "found a key in table that was never inserted");
        }
    }
}

/*
 Makes a  correctnes test of a table using a list of random numbers
 table - the table to do the lookups in
 seq - a list of (unique) random numbers to be used as keys and values
 */
void correctnessTest(Table *table,int seq[]) {

    int index = 0;
    test("isempty() == True"), check(table_isEmpty(table));

    comprehensiveTest("Simple tests");
    indenting();
    test("Attempt to lookup a nonexistent item");
    check(NULL == table_lookup(table,&seq[index]));

    test("Insert an item");
    int insertedKey = seq[index];
    index = index + 1;
    int insertedValue = seq[index];
    index = index + 1;
    table_insert(table,intPtrFromInt(insertedKey), intPtrFromInt(insertedValue));
    check(true);

    test("isempty() == False");
    check(!table_isEmpty(table));

    test("Lookup item");
    int *lookedUpValue = table_lookup(table,&insertedKey);
    check(lookedUpValue != NULL);
    test("Check value");
    check(*lookedUpValue == insertedValue);

    test("Attempt to lookup a nonexistent item");
    int nonexistentKey = seq[index];
    check(table_lookup(table,&nonexistentKey) == NULL);
    test("Attemt to remove a nonexistent item");
    table_remove(table,&nonexistentKey);
    check(true);
    test("isempty() == False");
    check(!table_isEmpty(table));

    test("Remove the inserted item");
    table_remove(table,&insertedKey);
    check(true);
    test("isempty() == True");
    check(table_isEmpty(table));
    outdenting();

    int nItems[] = {(int)(0.1*TABLESIZE), (int)(0.65*TABLESIZE), TABLESIZE};
    for(int iteration=0;iteration<3;iteration++){
        char str[100];
        sprintf(str,"Comprehensive test for %d  items\n",nItems[iteration]);
        comprehensiveTest(str);

        indenting();

        index = 0;
        int *keys = malloc(2*nItems[iteration]*sizeof(int));
        int *values = malloc(2*nItems[iteration]*sizeof(int));
        bool *inserted = calloc(2*nItems[iteration],sizeof(bool));
        for(int i=0;i<2*nItems[iteration];i++) {
            keys[i] = seq[index];
            values[i] = seq[index];
            index = index + 1;
        }

        char str2[100];
        sprintf(str2,"Insert %d items\n",nItems[iteration]);
        test(str2);
        for(int i=0;i<nItems[iteration];i++) {
            table_insert(table,intPtrFromInt(keys[i]), intPtrFromInt(values[i]));
            inserted[i] = true;
        }
        check(true);
        test("isempty() == False");
        check(!table_isEmpty(table));
        char str4[100];
        sprintf(str4,"Lookup %d items\n", 2*nItems[iteration]);
        test(str4);
        testLookups(keys, values, inserted, 2*nItems[iteration], table);
        check(true);

        test("Remove half of the inserted pairs");
        int half = nItems[iteration]/2;

        for(int i=0;i<nItems[iteration]/2;i++) {
            int j = (i+1)*(7652413%half);
            table_remove(table,&keys[j%half]);
            inserted[j%half] = false;
        }

        check(true);
        test("isempty() == False");
        check(!table_isEmpty(table));

        test("Look up all originally inserted items");
        testLookups(keys, values, inserted, nItems[iteration], table);
        check(true);

        test("Remove remaining items");
        int rest = nItems[iteration]-half;

        for(int i=0;i<rest;i++) {
            int j = (i+1)*(7652413%rest);
            table_remove(table,&keys[half+(j%rest)]);
            inserted[half+(j%rest)] = false;
        }
        check(true);
        test("isempty() == True");
        check(table_isEmpty(table));
        outdenting();
        free(keys);
        free(values);
        free(inserted);
    }
    outdenting();
    index = 0;

}


/*
 Makes a test of the speed of a table using a list of random numbers
 table - the table to do the test on
 seq - a list of (unique) random numbers to be used as keys and values
 */
void speedTest(Table *table,int seq[]) {

    int index = 0;
    int *keys = malloc(TABLESIZE*sizeof(int));
    int *values = malloc(TABLESIZE*sizeof(int));
    unsigned long start;
    unsigned long end;
    for(int i=0;i<TABLESIZE;i++) {
        keys[i] = seq[index];
        index = index + 1;
        values[i] = seq[index];
        index = index + 1;
    }
    // Insert all items
    printf("Insert %d items: \n", TABLESIZE);
    start = get_milliseconds();
    for(int i=0;i<TABLESIZE;i++) {
        table_insert(table,intPtrFromInt(keys[i]), intPtrFromInt(values[i]));
    }
    end =  get_milliseconds();
    printf("%lu ms.\n",end-start);

    // Lookup randomly
    printf("%d random lookups: \n",NROFACCESSES );
    start = get_milliseconds();
    for(int i=0;i<NROFACCESSES;i++) {
        int pos = rand()%TABLESIZE;
        table_lookup(table,&keys[pos]);
    }
    end = get_milliseconds();
    printf("%lu ms.\n", end-start);

    // Lookup skewed to a certain range
    int partition = TABLESIZE/4;
    int offset = 3*(rand()%partition) + 1;
    printf("%d skewed lookups: \n" ,NROFACCESSES);
    start =  get_milliseconds();
    for(int i=0;i<NROFACCESSES;i++) {
        int pos = rand()%partition + offset;
        table_lookup(table,&keys[pos]);
    }
    end = get_milliseconds();
    printf("%lu ms.\n" ,end-start);

    // Remove all items
    printf("Remove all items: \n");
    start = get_milliseconds();
    for(int i=0;i<TABLESIZE;i++) {
        int j = (i+1)*(7652413%TABLESIZE);
        table_remove(table,&keys[j%TABLESIZE]);
    }
    end = get_milliseconds();
    printf("%lu ms.\n" ,end-start);
    free(keys);
    free(values);
}

/* Generate n unique random numbers. The numbers will be stored i seq */
void randomsample(int seq[], int n) {
    int values[RANDOMRANGE];
    for(int i=0;i<RANDOMRANGE;i++) {
        values[i]=i;
    }
    for(int i=0;i<RANDOMRANGE;i++) {
        int switchPos=rand()%RANDOMRANGE;
        if(i!=switchPos) {
            int temp=values[i];
            values[i]=values[switchPos];
            values[switchPos]=temp;
        }
    }
    for(int i=0;i<n;i++) {
        seq[i]=values[i];
    }
}

/*
 Makes a test that a table is handling doubles of keys correctly
 table - the table to do the test on
 */
void doubleTest() {
    Table *table=table_create(compareInt);
    comprehensiveTest("Inserting the same key multiple times");
    int key=1;
    int val=1;
    int val2=2;
    int val3=3;

    table_insert(table,&key,&val);
    table_insert(table,&key,&val2);
    table_insert(table,&key,&val3);
    indenting();
    test("Attempt to lookup last inserted");
    val = *(int*)table_lookup(table,&key);
    check(3 == val);

    test("Removing key");
    table_remove(table,&key);
    test("Attempt to lookup removed");
    check(NULL == table_lookup(table,&key));
    outdenting();
    table_free(table);
}

/* Function that runs all the tests */
void testing() {
    Table *table = table_create(compareInt);
    table_setKeyMemHandler(table,free);
    table_setValueMemHandler(table,free);
    int seq[TABLESIZE*3];
    randomsample(seq, TABLESIZE*3);

    comprehensiveTest("Correctness test");
    indenting();
    correctnessTest(table, seq);
    outdenting();

    doubleTest();
    printf("All correctness tests succeeded! Press enter to continue!\n");
    getchar();

    comprehensiveTest("Running speed test");
    speedTest(table, seq);
    table_free(table);
}

int main(int argc, char *argv[]) {

    testing();
    printf("Test completed.\n");
    return 0;
}





