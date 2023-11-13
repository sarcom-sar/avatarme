#ifndef IDENTICON
#define IDENTICON

#include <stdint.h>

struct identicon_info {
  char magic_number[4];
  char size[5];
  char available_colors[5];
  uint8_t fg[3];
  uint8_t bg[3];
  uint8_t identicon[4][4*3];
};

void identicon_info_init_colors(struct identicon_info* ii, uint8_t mdv[]);
void identicon_info_build_picture(struct identicon_info* ii, uint8_t mdv[]);

int identicon_info_to_buffer(struct identicon_info ii, char** buf);

void identicon_info_print(char* buffer);

int identicon_info_write_to_file(char* buffer, const char* filename);

int write_to_buff(char** buf, const char* source, int* b_size, int* b_num);
#endif
