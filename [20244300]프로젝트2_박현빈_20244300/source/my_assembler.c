// my_assembler.c
// 시스템프로그래밍 프로젝트2 - 구현부

#include "my_assembler.h"

// 전역 변수 선언
char *input_data[MAX_LINES];
int line_num = 0;

token *token_table[MAX_LINES];
int token_line = 0;

inst *inst_table[MAX_INST];
int inst_index = 0;

// inst.data 파일 읽어서 테이블 만들기
void init_inst_table(char *filename) {
    FILE *fp;
    char line[100];
    char inst_name[10], format_type[5];
    int format;
    unsigned int opcode;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    inst_index = 0;

    // 파일 읽기
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 탭으로 구분된 데이터 파싱
        if (sscanf(line, "%s\t%s\t%d\t%X", inst_name, format_type, &format, &opcode) == 4) {
            inst_table[inst_index] = (inst *)malloc(sizeof(inst));

            strcpy(inst_table[inst_index]->str, inst_name);
            inst_table[inst_index]->op = (unsigned char)opcode;
            inst_table[inst_index]->format = format;
            inst_table[inst_index]->ops = format;

            inst_index++;

            if (inst_index >= MAX_INST)
                break;
        }
    }
    fclose(fp);
}

// opcode 찾기
int search_opcode(char *str) {
    int i;

    if (str == NULL)
        return -1;

    for (i = 0; i < inst_index; i++) {
        if (strcmp(inst_table[i]->str, str) == 0) {
            return inst_table[i]->op;
        }
    }
    return -1;  // 못찾음
}

// 입력파일 읽기
void load_input_file(char *filename) {
    FILE *fp;
    char line[200];

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    line_num = 0;
    while (fgets(line, sizeof(line), fp) != NULL && line_num < MAX_LINES) {
        // 개행문자 제거
        line[strcspn(line, "\n")] = '\0';

        input_data[line_num] = (char *)malloc(strlen(line) + 1);
        strcpy(input_data[line_num], line);
        line_num++;
    }
    fclose(fp);
}

// 토큰 파싱
void token_parsing(void) {
    int i, j;
    char *line_copy;
    char *token_ptr;
    int operand_cnt;

    token_line = 0;

    for (i = 0; i < line_num; i++) {
        // 빈줄이나 주석 건너뛰기
        if (input_data[i][0] == '\0' || input_data[i][0] == '.')
            continue;

        token_table[token_line] = (token *)malloc(sizeof(token));

        // 초기화
        token_table[token_line]->label = NULL;
        token_table[token_line]->operation = NULL;
        for (j = 0; j < MAX_OPERAND; j++) {
            token_table[token_line]->operand[j][0] = '\0';
        }
        token_table[token_line]->comment[0] = '\0';

        line_copy = (char *)malloc(strlen(input_data[i]) + 1);
        strcpy(line_copy, input_data[i]);

        // 주석 분리
        char *comment_pos = strchr(line_copy, '.');
        if (comment_pos != NULL) {
            strcpy(token_table[token_line]->comment, comment_pos);
            *comment_pos = '\0';
        }

        // label 있는지 확인 (첫글자가 공백 아니면 label)
        if (line_copy[0] != ' ' && line_copy[0] != '\t' && line_copy[0] != '\0') {
            token_ptr = strtok(line_copy, " \t");
            if (token_ptr != NULL) {
                token_table[token_line]->label = (char *)malloc(strlen(token_ptr) + 1);
                strcpy(token_table[token_line]->label, token_ptr);
                token_ptr = strtok(NULL, " \t");
            }
        } else {
            token_ptr = strtok(line_copy, " \t");
        }

        // operation 파싱
        if (token_ptr != NULL && token_ptr[0] != '.') {
            token_table[token_line]->operation = (char *)malloc(strlen(token_ptr) + 1);
            strcpy(token_table[token_line]->operation, token_ptr);
        }

        // operand 파싱 (콤마로 구분)
        token_ptr = strtok(NULL, " \t");
        operand_cnt = 0;

        if (token_ptr != NULL && token_ptr[0] != '.') {
            char *operand_copy = (char *)malloc(strlen(token_ptr) + 1);
            strcpy(operand_copy, token_ptr);

            char *op_token = strtok(operand_copy, ",");
            while (op_token != NULL && operand_cnt < MAX_OPERAND) {
                // 공백 제거
                while (*op_token == ' ' || *op_token == '\t')
                    op_token++;

                strncpy(token_table[token_line]->operand[operand_cnt], op_token, 19);
                token_table[token_line]->operand[operand_cnt][19] = '\0';
                operand_cnt++;

                op_token = strtok(NULL, ",");
            }
            free(operand_copy);
        }

        free(line_copy);
        token_line++;
    }
}

// 출력
void print_output(void) {
    int i, j;
    int opcode;

    for (i = 0; i < token_line; i++) {
        // label
        if (token_table[i]->label != NULL) {
            printf("%-8s", token_table[i]->label);
        } else {
            printf("        ");  // 8칸
        }

        // operator
        if (token_table[i]->operation != NULL) {
            printf("%-8s", token_table[i]->operation);
        }

        // operand
        for (j = 0; j < MAX_OPERAND; j++) {
            if (token_table[i]->operand[j][0] != '\0') {
                if (j > 0) printf(",");
                printf("%s", token_table[i]->operand[j]);
            }
        }

        // opcode (hex)
        if (token_table[i]->operation != NULL) {
            opcode = search_opcode(token_table[i]->operation);
            if (opcode >= 0) {
                printf("\t\t%02X", opcode);
            }
        }
        printf("\n");
    }
}