#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include "structure.h"
#include "operation.h"
#include "timecount.h"

int main(int argc, char *argv[]) {
    set_query(argv[1]);
    set_table(argv[1]);

    create();
    init8bit();
    setting_fast();
    setting_seg();
    search_process();

    return 0;
}
