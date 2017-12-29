#include "generator.h"

#include "opcodes.h"
#include "flags.h"
#include "tree.h"
#include "files.h"

#include <string.h>

static struct node* labels;

static void destruct()
{
    tree.burn(labels);
}

static void construct()
{
    atexit(destruct);
}

static void entrance(const char* entry)
{
    struct node* reset = tree.get(labels, entry);
    if(!reset)
    {
        fprintf(stderr, "error: entry point %s not found\n", entry);
        exit(1);
    }
    fprintf(files.output(), "1%03X\n", reset->address);
}

static void scan(bool growing)
{
    unsigned address = 0x0202;
    #define MAX 320
    char line[MAX];
    for(unsigned linenumber = 1; fgets(line, MAX, files.input()); linenumber++)
    {
        char* label;
        char* mnemonic;
        char* operand;
        char* newline;
        char* semicolon;
        char* colon;
        int error = 0;
        newline = strchr(line, '\n');
        if(newline)
            *newline = '\0';
        semicolon = strchr(line, ';');
        if(semicolon)
            *semicolon = '\0';
        colon = strchr(line, ':');
        if(colon)
        {
            label = strtok(line, "\t :");
            if(growing)
            {
                labels = tree.insert(labels, tree.build(label, address));
                if(flags.bits.tree)
                {
                    fprintf(stderr, "error: line %d: %s already defined\n", linenumber, label);
                    exit(1);
                }
            }
        }
        mnemonic = strtok(colon ? NULL : line, "\t ");
        operand = strtok(NULL, "");
        if(mnemonic)
        {
            if(growing)
                address += strcmp(mnemonic, "DB") == 0 ? 0x0001 : 0x0002;
            else
                error = opcodes.assemble(mnemonic, operand, labels);
        }
        if(error)
        {
            const char* types[] = {
                /* 0 */ "no error",
                /* 1 */ "operand formatting",
                /* 2 */ "label not found",
                /* 3 */ "unsupported chip8 mnemonic"
            };
            flags.bits.generator = true;
            fprintf(stderr, "error: line %d: %s\n", linenumber, types[error]);
            exit(1);
        }
    }
}

const struct generator generator = { construct, entrance, scan, };
