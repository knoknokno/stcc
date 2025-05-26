#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==========Token==========
typedef enum {
    TK_RESERVED,
    TK_RETURN,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token* next;
    int val;
    char* str;
    int len;
};

typedef struct LVar LVar;

struct LVar {
    LVar* next;
    char* name;
    int len;
    int offset;
};

bool consume(char* op);
void expect(char* p);
int expect_number();
bool at_eof();
Token* new_token(TokenKind, Token*, char*, int);
bool startswith(char*, char*);
Token* tokenize(char* p);
LVar* find_lvar(Token* tok);

// ==========Node==========
typedef enum {
    ND_RETURN,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_ASSIGN,
    ND_LVAR,
    ND_NUM
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val;
    int offset; // ローカル変数の場合のベースポインタからのオフセット
};

Node* new_node(NodeKind, Node*, Node*);
Node* new_node_num(int);

void program();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* rerational();
Node* add();
Node* mul();
Node* unary();
Node* primary();

// ==========CodeGen==========
void gen(Node*);

// ==========Error==========
void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);

// ==========Global==========

extern char* user_input;

extern Token* token;

extern Node* code[100];

extern LVar* locals;
