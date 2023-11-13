#include "avatarme.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char identifier;
  const char *value;
  cag_option_context context;
  struct config conf = {NULL};
  uint8_t param_index;
  uint8_t md_value[EVP_MAX_MD_SIZE];
  struct identicon_info iden_i;

  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    identifier = cag_option_get(&context);
    switch (identifier) {
    case 'h':
      printf("Syntax: avatarme [OPTION] STRING\n");
      printf("Outputs identicon made from STRING into default file\n");
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      return 0;
    case 'o':
      value = cag_option_get_value(&context);
      if (value == NULL) conf.out = "iden.bmp"; else conf.out = value;
      break;
    }
  }

  // horrible, horrible hack to get STRING no matter what
  for (param_index = context.index; param_index < argc; ++param_index) {
    break;
  }
  if (argv[param_index] == NULL) {
    printf("No input\n");
    return -1;
  }

  if (get_md5_digest((uint8_t *)argv[param_index], md_value)) {
    return -1;
  }

  identicon_info_init_colors(&iden_i, md_value);

  identicon_info_build_picture(&iden_i, md_value);


  char *buffer = malloc(1024);
  if (identicon_info_to_buffer(iden_i, &buffer) < 0) {
    printf("malloc failed\n");
    return -1;
  }

  if (conf.out == NULL) {
    identicon_info_print(buffer);
  } else {
    if (identicon_info_write_to_file(buffer, conf.out) < 0) {
      printf("failed to write to specified file\n");
      return -1;
    }
  }

  return 0;
}

uint8_t get_md5_digest(uint8_t *input, uint8_t *digest) {
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;
  uint32_t md_len;

  md = EVP_get_digestbyname("md5");
  mdctx = EVP_MD_CTX_new();

  if (!EVP_DigestInit_ex2(mdctx, md, NULL)) {
    printf("Message init failed\n");
    goto failure;
  }
  if (!EVP_DigestUpdate(mdctx, input, strlen((const char *)input))) {
    printf("Message digest update failed\n");
    goto failure;
  }
  if (!EVP_DigestFinal(mdctx, digest, &md_len)) {
    printf("Message digest finalisation failed\n");
    goto failure;
  }
  EVP_MD_CTX_free(mdctx);

  return 0;

failure:
  EVP_MD_CTX_free(mdctx);
  return -1;
}

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
  for (int i = 0; ii.magic_number[i] != '\0'; i++) {
    (*buf)[buff_num] = ii.magic_number[i];
    if (buff_num >= buff_size) {
      *buf = realloc(*buf, buff_size *= 2);
      if (!(*buf)) return -1;
    }
    buff_num++;
  }
  for (int i = 0; ii.size[i] != '\0'; i++) {
    (*buf)[buff_num] = ii.size[i];
    if (buff_num >= buff_size) {
      *buf = realloc(*buf, buff_size *= 2);
      if (!(*buf))
        return -1;
    }
    buff_num++;
  }
  for (int i = 0; ii.available_colors[i] != '\0'; i++) {
    (*buf)[buff_num] = ii.available_colors[i];
    if (buff_num >= buff_size) {
      *buf = realloc(*buf, buff_size *= 2);
      if (!(*buf))
        return -1;
    }
    buff_num++;
  }
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
