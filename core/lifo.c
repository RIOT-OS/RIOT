#include <lifo.h>

int lifo_empty(int *array)
{
    return array[0] == -1;
}

void lifo_init(int *array, int n)
{
    for(int i = 0; i <= n; i++) {
        array[i] = -1;
    }
}

void lifo_insert(int *array, int i)
{
    int index = i + 1;
    array[index] = array[0];
    array[0] = i;
}

int lifo_get(int *array)
{
    int head = array[0];

    if(head != -1) {
        array[0] = array[head + 1];
    }

    return head;
}


#ifdef WITH_MAIN
#include <stdio.h>
int main()
{
    int array[5];

    lifo_init(array, 4);

    lifo_insert(array, 0);
    lifo_insert(array, 1);
    lifo_insert(array, 2);
    lifo_insert(array, 3);
    printf("get: %i\n", lifo_get(array));


    return 0;
}
#endif
