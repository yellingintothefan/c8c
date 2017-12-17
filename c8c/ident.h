#pragma once

typedef struct node Node;

struct node
{
    Node* next;
    int rp;
    char* name;
};

struct ident
{
    void (*init)(void);
    Node* (*create)(char* name, const int rp);
    Node* (*push)(Node* const node);
    Node (*pop)(void);
    Node* (*find)(char* name);
};

extern const struct ident ident;
