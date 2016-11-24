/*
 *
 *
 *
 */
#include <stdio.h>

typedef int (*intfunc_t)();

typedef struct _tag_sql {
    intfunc_t init;
} sql_t;

int init ();

sql_t sql = {
    init
};

int main (int argc, char ** argv)
{
    sql.init ();

    return 0;
}

int init ()
{
    printf ("La funcion 'init' ha sido llamada.\n");

    return 0;
}
