// my_assembler.h
// 시스템프로그래밍 프로젝트2 - SIC 어셈블러 파서

#ifndef MY_ASSEMBLER_H
#define MY_ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 상수 정의
#define MAX_LINES 5000
#define MAX_INST 256
#define MAX_OPERAND 3

// 전역 변수
extern char *input_data[MAX_LINES];
extern int line_num;

// 토큰 구조체
struct token_unit {
    char *label;
    char *operation;
    char operand[MAX_OPERAND][20];
    char comment[100];
};

typedef struct token_unit token;
extern token *token_table[MAX_LINES];
extern int token_line;

// instruction 구조체
struct inst_unit {
    char str[10];
    unsigned char op;
    int format;
    int ops;
};

typedef struct inst_unit inst;
extern inst *inst_table[MAX_INST];
extern int inst_index;

// 함수 선언
void init_inst_table(char *filename);
int search_opcode(char *str);
void load_input_file(char *filename);
void token_parsing(void);
void print_output(void);

#endif