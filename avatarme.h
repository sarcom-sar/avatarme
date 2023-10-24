#ifndef avatarme
#define avatarme

#include <cargs.h>
#include <openssl/evp.h>

static struct cag_option options[] = {
    {.identifier = 'h',
     .access_letters = "h",
     .access_name = "help",
     .description = "Shows commands\' help"},
    {.identifier = 'o',
     .access_letters = "o",
     .access_name = "out",
     .value_name = "OUT",
     .description = "Specifies output file, defaults to iden.bmp"}
};

struct config {
  const char* out;
};

#endif