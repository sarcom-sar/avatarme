#include "avatarme.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char identifier;
  const char *value;
  cag_option_context context;
  struct config conf = {"iden.bmp"};
  uint8_t param_index;
  uint8_t md_value[EVP_MAX_MD_SIZE];

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
  for (int i = 0; i < 16; i++) {
    printf("%02x", md_value[i]);
  }
  printf("\n");
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
  if (!EVP_DigestUpdate(mdctx, input, strlen((const char*)input))) {
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
