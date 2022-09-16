#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'}, // multiple
  {"\\/", '/'}, // divide
  {"\\(", '('},
  {"\\)", ')'},        // colon
  {"==", TK_EQ},        // equal
  {"([0-9])*", TK_NUM},

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: {
            // do nothing 
            break;
          }
          case TK_EQ: {
            TODO();
            break;
          }
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')': 
          case TK_NUM:
          {
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            break;
          }
          default: TODO();
        }

        // break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

#define is_op(x) (x=='+' || x=='-' || x=='*' || x=='/') ? true : false
#define lower_eq_op(x) (x=='+' || x=='-') ? true : false // lower or equal

int find_parentheses(int p){
  int pair_count = 0;
  for(int i=p; i<nr_token; ++i){
    if(tokens[i].type == '('){
      pair_count++;
    }
    if(tokens[i].type == ')') {
      pair_count--;
    }
    if(pair_count == 0) {
      return i;
    }
  }
  return 0;
}

int find_main_operator(int p, int q){
  assert(p < q);
  int main_op = -1;
  for(int i=p; i<=q;++i) {
    if(tokens[i].type == '(') {
      i += find_parentheses(i);
      continue;
    }
    if(!is_op(tokens[i].type)) {
      continue;
    }
    if(main_op < 0 || lower_eq_op(tokens[i].type)) {
      main_op = i;
    }
  }
  return main_op;
}

bool check_parentheses(int p, int q) {
  if(tokens[p].type != '(' || tokens[q].type != ')') 
    return false;
  return find_parentheses(p) == q;
}

word_t eval(int p, int q, bool *success) {
  if(!success) return 0;

  if (p > q) {
    *success = false;
    return 0;
  }
  else if (p == q) {
    *success = true;
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1, success);
  }
  else {
    int main_op = find_main_operator(p, q);
    Log("p: %d, q: %d, main op: %d\n",p,q, main_op);
    // if(main_op<0)
    word_t val1 = eval(p, main_op - 1, success);
    word_t val2 = eval(main_op + 1, q, success);

    switch (tokens[main_op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  return eval(0, nr_token-1, success);
}
