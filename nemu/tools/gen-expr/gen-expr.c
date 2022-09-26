#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static uint32_t expr_len = 0;

uint32_t choose(uint32_t s) {
  return rand() % s;
}

static void get_num() {
  buf[expr_len++] = choose(10) + '0';
}

static void gen(char s){
  buf[expr_len++] = s;
}

static void gen_rand_op() {
  switch (choose(4)) {
  case 0: gen('+'); break;
  case 1: gen('-'); break;
  case 2: gen('*'); break;
  case 3: gen('/'); break;
  // default: assert(true); break;
  }
}

static void gen_rand_expr() {
  switch (choose(3)) {
    case 0: get_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

static void clear_buf() {
  memset(buf, 0, sizeof buf);
  expr_len = 0;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    buf[expr_len] = '\0';

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -Werror -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    int fsize = fscanf(fp, "%d", &result);
//  printf("raw result: %d\n", result);
    pclose(fp);
    if(fsize >= 0) {
      printf("%u %s\n", result, buf);
    }
    clear_buf();
  }
  return 0;
}
