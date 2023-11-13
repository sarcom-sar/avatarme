#include "identicon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void identicon_info_init_colors(struct identicon_info* ii, uint8_t mdv[]) {
  strcpy(ii->magic_number, "P3\n");
  strcpy(ii->size, "8 8\n");
  strcpy(ii->available_colors, "255\n");
  for (int i = 0; i < 3; i++) {
    ii->bg[i] = mdv[i];
    ii->fg[i] = mdv[i+3];
  }
}

void identicon_info_build_picture(struct identicon_info* ii, uint8_t mdv[]) {
  // first 4 pixels in 1st quadrant
  int x = 0;
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 12; i += 3) {
      if (mdv[x] % 4 == 0) {
        ii->identicon[j][i] = ii->fg[0];
        ii->identicon[j][i + 1] = ii->fg[1];
        ii->identicon[j][i + 2] = ii->fg[2];
      } else {
        ii->identicon[j][i] = ii->bg[0];
        ii->identicon[j][i + 1] = ii->bg[1];
        ii->identicon[j][i + 2] = ii->bg[2];
      }
      x++;
    }
  }
}


int identicon_info_to_buffer(struct identicon_info ii, char** buf) {
  int buff_size = 1024;
  int buff_num = 0;
  char temp_buffer[13]; // guaranteed to be at best
                        // "255 255 255 "
  // boilerplate
  write_to_buff(buf, ii.magic_number, &buff_size, &buff_num);
  write_to_buff(buf, ii.size, &buff_size, &buff_num);
  write_to_buff(buf, ii.available_colors, &buff_size, &buff_num);

  // writes both 1st and 2nd quadrant to buffer
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 12; i++) {
      sprintf(temp_buffer, "%d ", ii.identicon[j][i]);
      for (int x = 0; temp_buffer[x] != '\0'; x++) {
        (*buf)[buff_num++] = temp_buffer[x];
        if (buff_num >= buff_size) {
          *buf = realloc(*buf, buff_size *= 2);
          if (!(*buf))
            return -1;
        }
      }
    }
    for (int i = 4; i > 0; i--) {
      int v = i * 3;
      sprintf(temp_buffer, "%d %d %d ", ii.identicon[j][v - 3],
              ii.identicon[j][v - 2], ii.identicon[j][v - 1]);
      for (int x = 0; temp_buffer[x] != '\0'; x++) {
        (*buf)[buff_num++] = temp_buffer[x];
        if (buff_num >= buff_size) {
          *buf = realloc(*buf, buff_size *= 2);
          if (!(*buf))
            return -1;
        }
      }
    }
  }

  // writes 3rd and 4th quadrant
  for (int j = 3; j >= 0; j--) {
    for (int i = 0; i < 12; i++) {
      sprintf(temp_buffer, "%d ", ii.identicon[j][i]);
      for (int x = 0; temp_buffer[x] != '\0'; x++) {
        (*buf)[buff_num++] = temp_buffer[x];
        if (buff_num >= buff_size) {
          *buf = realloc(*buf, buff_size *= 2);
          if (!(*buf))
            return -1;
        }
      }
    }
    for (int i = 4; i > 0; i--) {
      int v = i * 3;
      sprintf(temp_buffer, "%d %d %d ", ii.identicon[j][v - 3],
              ii.identicon[j][v - 2], ii.identicon[j][v - 1]);
      for (int x = 0; temp_buffer[x] != '\0'; x++) {
        (*buf)[buff_num++] = temp_buffer[x];
        if (buff_num >= buff_size) {
          *buf = realloc(*buf, buff_size *= 2);
          if (!(*buf))
            return -1;
        }
      }
    }
  }
  (*buf)[buff_num] = '\0';
  return 0;
}

void identicon_info_print(char *buffer) {
  printf("%s\n", buffer);
}

int identicon_info_write_to_file(char* buffer, const char *filename) {
  FILE* fptr;
  fptr = fopen(filename, "w");
  if (fptr == NULL) return -1;

  fprintf(fptr, "%s", buffer);
  fclose(fptr);
  return 0;
}

int write_to_buff(char** buf, const char* source, int* b_size, int* b_num) {
  for (int i = 0; source[i] != '\0'; i++) {
    (*buf)[(*b_num)++] = source[i];
    if ((*b_num) >= (*b_size)) {
      (*buf) = realloc(*buf, (*b_size) *= 2);
      if (!(*buf)) return -1;
    }
  }
  return 0;
}
