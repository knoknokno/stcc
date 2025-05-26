#include "stcc.h"

char* user_input;

Token* token;

Node* code[100];

LVar* locals;

int main(int argc, char** argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // プロローグ
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // a-zの26文字分ローカル変数用の領域

  // 先頭の式からコード生成
  for (int i = 0; code[i]; i++) {
      gen(code[i]);

      printf("  pop rax\n"); // 式の評価結果をポップ
  }

  // エピローグ
  // 最後の式の評価結果がraxにあるので、それが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}