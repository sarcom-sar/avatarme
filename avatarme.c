#include "avatarme.h"
#include "identicon.h"
#include <openssl/evp.h>
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

