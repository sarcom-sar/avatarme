#ifndef avatarme
#define avatarme

#include <stdint.h>
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

struct identicon_info {
  char magic_number[4];
  char size[5];
  char available_colors[5];
  uint8_t fg[3];
  uint8_t bg[3];
  uint8_t identicon[4][4*3];
};

uint8_t get_md5_digest(uint8_t* input, uint8_t* digest);
void identicon_info_init_colors(struct identicon_info* ii, uint8_t mdv[]);
int identicon_info_build_picture(struct identicon_info* ii, uint8_t mdv[]);

void identicon_info_print_stuff(struct identicon_info* ii);

#endif
