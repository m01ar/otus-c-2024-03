#include <stdio.h>
#include <stdlib.h>

/// Пустая строка для функции puts
char *empty_str = "";
/// Форматная строка для функции print_int
char *int_format = "%ld ";

/// Входной массив чисел
long data[] = {4, 8, 15, 16, 23, 42};
/// Размер массива
int data_length = sizeof data / sizeof *data;

/// Структура, определяющая элемент связного списка
typedef struct Node {
    long value;
    struct Node *next;
} node_t;

/// НАЗНАЧЕНИЕ:
///   Выводит число в консоль с помощью printf
///   по форматной строке, заданной в глобальной переменной
/// ПАРАМЕТРЫ:
///   arg1 (long) - выводимое число

// void print_int(rdi=arg1)
void print_int(long arg1)
{
    printf(int_format, arg1); // (rdi=int_format, rsi=<rdi>)
    fflush(0); // (rdi=0)
}

/// НАЗНАЧЕНИЕ:
///   Предикат. Проверяет число на нечётность.
/// ПАРАМЕТРЫ:
///   arg1 (long) - проверяемое число
/// ВОЗВРАТ:
///   (int) : 1 если нечётное, 0 в противном случае

// int p(rdi=arg1)
int is_odd(long arg1) // aka "p"
{
    return arg1 & 1;
}

/// НАЗНАЧЕНИЕ:
///   Добавляет элемент к связному списку.
/// ПАРАМЕТРЫ:
///   value (long)    - добавляемое значение
///   list (node_t *) - принимающий список
/// ВОЗВРАТ:
///   (node_t *) - голова списка после добавления элемента

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

/// НАЗНАЧЕНИЕ:
///   Первоначально функция map предназначалась для отображения одного
///   списка в другой путём применения функции к каждому из элементов списка.
///   Данная реализация новый список не создаёт, а только применяет функцию.
///   Возможно, более подходящее название для этой функции - apply.
/// ПАРАМЕТРЫ:
///   list (node_t *) - список, к которому будет применяться функция.
///   fn (void (*)(long)) - указатель на применяемую функцию.

// void map(rdi=list, rsi=fn)
void map(node_t *list, void (*fn)(long)) // aka "m"
{
    if (!list)  // test rdi, rdi
        return; // jz outm ; -> ret

    fn(list->value); // rsi(rdi=[<rdi>])
    map(list->next, fn); // m(rdi=[<rdi>+8], rsi=<rsi>)
}

/// Вариант реализации полноценной функции map:

/*
node_t *map(long (*fn)(long), node_t *list)
{
    if (list == 0)
        return 0;

    return add_element(fn(list->value), map(fn, list->next));
}
*/

/// НАЗНАЧЕНИЕ:
///   Фильтр: применяет функцию-предикат ко входному списку и формирует
///   выходной список из тех элементов входного списка, по которым
///   функция-предикат вернула истину.
/// ПАРАМЕТРЫ:
///   list  (note_t *)      - обрабатываемый список
///   list2 (node_t *)      - указатель для рекурсивного вызова
///   fn    (int (*)(long)) - указатель на функцию-предикат
/// ВОЗВРАТ:
///   (node_t *) - голова нового сформированного списка

// node_t *filter(rdi=list, rsi=list2=0, rdx=fn)
node_t *filter(node_t *list, node_t *list2, int (*fn)(long)) // aka "f"
{
    if (list == NULL)
        return list2;

    if (fn(list->value))
        return filter(list->next, add_element(list->value, list2), fn);
    else
        return filter(list->next, list2, fn);
}

/// В качестве оптимизации можно предложить сократить количество параметров
/// Например так:

/*
node_t *filter(int (*fn)(long), node_t *list)
{
    if (list == 0)
        return 0;
    if (fn(n->value))
        return add_element(list->value, filter(fn, list->next));
    return filter(fn, list->next);
}
*/

/// Вариант функции, реализующей освобождение памяти,
/// выделенной под связный список.

/*
void free_list(node_t *list)
{
    if (list->next)
        free_list(list->next);

    free(list);
}
*/

int main(void)
{
    int counter = data_length;
    node_t *list = NULL;

    do
    {
        list = add_element(data[--counter], list); // Выделение памяти
    }
    while (counter != 0);

    map(list, print_int); // Ассемблерная функция m
    puts(empty_str);

    // Выделение памяти
    list = filter(list, NULL, is_odd); // Ассемблерная функция f

    map(list, print_int);
    puts(empty_str);

    // Утечка памяти возникает из-за того, что память, выделяемая в отмеченных
    // местах, не освобождается по завершении работы программы.
    //
    // Для освобождения памяти, выделенной под связный спискок, необходимо
    // проследовать по списку до последнего элемента и освободить его память,
    // затем, поднимаясь в сторону головы, освобождать проходимые элементы.
    //
    // Освобождение памяти списка можно реализовать циклом, но на мой взгляд проще
    // рекурсивно. Вариант представлен функцией free_list.

    return 0;
}
