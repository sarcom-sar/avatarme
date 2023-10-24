#include "avatarme.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char identifier;
  const char *value;
  cag_option_context context;
  struct config conf = {"iden.bmp"};
  int param_index;

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
  return 0;
}
