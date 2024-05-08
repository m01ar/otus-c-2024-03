#include <stdio.h>
#include <stdlib.h>

char *empty_str = "";
char *int_format = "%ld ";

long data[] = {4, 8, 15, 16, 23, 42};
int data_length = sizeof data / sizeof *data;

typedef struct Node {
    long value;
    struct Node *next;
} node_t;

// void print_int(rdi=arg1)
void print_int(long arg1)
{
    printf(int_format, arg1); // (rdi=int_format, rsi=<rdi>)
    fflush(0); // (rdi=0)
}

// int p(rdi=arg1)
int is_odd(long arg1) // aka "p"
{
    return arg1 & 1;
}

// node_t *add_element(rdi=value, rsi=list)
node_t *add_element(long value, node_t *list)
{
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
        abort();

    new_node->value = value; // [rax]   <- <rdi>
    new_node->next  = list;  // [rax+8] <- <rsi>

    return new_node;
}

// void map(rdi=list, rsi=fn)
void map(node_t *list, void (*fn)(long)) // aka "m"
{
    if (!list)  // test rdi, rdi
        return; // jz outm ; -> ret

    fn(list->value); // rsi(rdi=[<rdi>])
    map(list->next, fn); // m(rdi=[<rdi>+8], rsi=<rsi>)
}

// node_t *filter(rdi=list, rsi=list2=0, rdx=fn)
node_t *filter(node_t *list, node_t *list2, int (*fn)(long)) // aka "f"
{
    if (list == NULL)
        return list2;

    if (fn(list->value))
//        return add_element(list->value, filter(list->next, 0, fn));
        return filter(list->next, add_element(list->value, list2), fn);
    else
        return filter(list->next, list2, fn);
}

int main(void)
{
    int counter = data_length;
    node_t *list = NULL;

    do
    {
        list = add_element(data[--counter], list);
    }
    while (counter != 0);

    map(list, print_int);
    puts(empty_str);

    list = filter(list, NULL, is_odd);

    map(list, print_int);
    puts(empty_str);

    return 0;
}
