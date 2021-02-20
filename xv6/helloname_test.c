#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    char *name = argv[1];

    hello_name(name);

    exit();
}
