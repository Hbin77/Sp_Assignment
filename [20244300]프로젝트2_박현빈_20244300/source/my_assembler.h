/*
* my_assembler.h
 * 시스템 프로그래밍 프로젝트 #2
 * SIC 어셈블러 파서 - 헤더 파일
 */

#ifndef MY_ASSEMBLER_H
#define MY_ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 상수 정의 */
#define MAX_LINES 5000      // 최대 소스 라인 수
#define MAX_INST 256        // 최대 instruction 수
#define MAX_OPERAND 3       // 최대 operand 개수

/* 전역 변수 선언 */

// 어셈블리 할 소스코드를 파일로부터 불러와 라인별로 관리하는 테이블 생성
extern char *input_data[MAX_LINES];

// 어셈블리 할 라인 별 소스코드를 토큰 단위로 관리하기 위한 구조체 변수
struct token_unit {
    char *label;                    // 명령어 라인 중 label
    char *operator;                 // 명령어 라인 중 operator
    char operand[MAX_OPERAND][20];  // 명령어 라인 중 operand
    char comment[100];              // 명령어 라인 중 comment
};

// 어셈블리 할 소스코드를 5000라인까지 관리하는 테이블 생성
typedef struct token_unit token;
extern token *token_table[MAX_LINES];
extern int token_line;

// Instruction을 관리하기 위한 구조체 변수
struct inst_unit {
    char str[10];           // instruction의 이름
    unsigned char op;       // 명령어의 OPCODE
    int format;             // instruction의 형식
    int ops;                // instruction의 operand 개수
};

// Instruction의 정보를 가진 구조체를 관리하는 테이블 생성
typedef struct inst_unit inst;
extern inst *inst_table[MAX_INST];
extern int inst_index;

/* 함수 프로토타입 선언 */

// inst.data 파일로부터 instruction 테이블 초기화
void init_inst_table(char *filename);

// instruction 테이블에서 해당 명령어의 OPCODE 검색
int search_opcode(char *str);

// 입력 파일을 읽어 input_data에 저장
void load_input_file(char *filename);

// 각 라인을 토큰으로 분리하여 token_table에 저장
void token_parsing(void);

// 토큰 테이블과 OPCODE를 출력
void print_output(void);

#endif /* MY_ASSEMBLER_H */