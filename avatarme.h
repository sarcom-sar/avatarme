#ifndef avatarme
#define avatarme

#include <cargs.h>
#include <stdint.h>

static struct cag_option options[] = {
    {.identifier = 'h',
     .access_letters = "h",
     .access_name = "help",
     .description = "Shows commands\' help"},
    {.identifier = 'o',
     .access_letters = "o",
     .access_name = "out",
     .value_name = "OUT",
     .description = "Specifies output file, otherwise prints to stdout"}};

struct config {
  const char *out;
};

uint8_t get_md5_digest(uint8_t *input, uint8_t *digest);

#endif
