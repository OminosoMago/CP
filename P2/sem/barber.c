#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "options.h"


int main (int argc, char **argv)
{
    struct options opt;

    // Default values for the options
    opt.barbers = 5;
    opt.customers = 1000;
    opt.cut_time  = 3000;

    read_options(argc, argv, &opt);

    printf("barbers:%d\ncustomers:%d\ncut_time5%d\n ",opt.barbers,opt.customers,opt.cut_time);

    exit (0);
}
