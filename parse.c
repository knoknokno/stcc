#include "stcc.h"

//==========Tokenizer==========

bool consume(char* op) {
    if (token->kind != TK_RESEVERD ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

Token* consume_ident() {
    if (token->kind != TK_IDENT) {
        return NULL;
    }
    Token* tok = token;
    token = token->next;
    return tok;
}

void expect(char* op) {
    if (token->kind != TK_RESEVERD ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        error_at(token->str, " is not %s", op);
    }
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "is not num");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char* p, char* q) {
    return memcmp(p, q, strlen(q)) == 0;
}

Token* tokenize(char* p) {
    Token head;
    head.next = NULL;
    Token* cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        else if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESEVERD, cur, p, 2);
            p += 2;
            continue;
        }
        else if (strchr("+-*/()<>=;", *p)) {
            cur = new_token(TK_RESEVERD, cur, p++, 1);
        }
        else if ('a' <= *p && *p <= 'z') {
            cur = new_token(TK_IDENT, cur, p++, 1);
            continue;
        }
        else if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char* q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }
        else {
            error("can't tokenize");
        }
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

//==========Node parser==========

Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_lvar(int offset) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = offset;
    return node;
}

Node* new_node_num(int val) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = stmt();
    }
    code[i] = NULL;

}

Node* stmt() {
    Node* node = expr();
    expect(";");
    return node;
}

Node* expr() {
    return assign();
}

Node* assign() {
    Node* node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node* equality() {
    Node* node = rerational();

    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ, node, rerational());
        }
        else if (consume("!=")) {
            node = new_node(ND_NE, node, rerational());
        }
        else {
            return node;
        }
    }
}

Node* rerational() {
    Node* node = add();

    for (;;) {
        if (consume("<")) {
            node = new_node(ND_LT, node, add());
        }
        else if (consume("<=")) {
            node = new_node(ND_LE, node, add());
        }
        else if (consume(">")) {
            node = new_node(ND_LT, add(), node);
        }
        else if (consume(">=")) {
            node = new_node(ND_LE, add(), node);
        }
        else {
            return node;
        }
    }
}

Node* add() {
    Node* node = mul();

    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD, node, mul());
        }
        else if (consume("-")) {
            node = new_node(ND_SUB, node, mul());
        }
        else {
            return node;
        }
    }
}

Node* mul() {
    Node* node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL, node, unary());
        }
        else if (consume("/")) {
            node = new_node(ND_DIV, node, unary());
        }
        else {
            return node;
        }
    }
}

Node* unary() {
    if (consume("+")) {
        return primary();
    }
    else if (consume("-")) {
        return new_node(ND_MUL, new_node_num(-1), primary());
    }
    else {
        return primary();
    }
}

Node* primary() {
    if (consume("(")) {
        Node* node = expr();
        expect(")");
        return node;
    }

    Token* tok = consume_ident();
    if (tok) {
        Node* node = new_node_lvar((tok->str[0] - 'a' + 1) * 8);
        return node;
    }
    else {
        return new_node_num(expect_number());
    }
}

