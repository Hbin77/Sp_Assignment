#include "my_assembler.h"

/* 전역 변수 정의 */

// 어셈블리 할 소스코드를 파일로부터 불러와 라인별로 관리하는 테이블 생성
char *input_data[MAX_LINES];
static int line_num = 0;

// 어셈블리 할 소스코드를 5000라인까지 관리하는 테이블 생성
token *token_table[MAX_LINES];
int token_line = 0;

// Instruction의 정보를 가진 구조체를 관리하는 테이블 생성
inst *inst_table[MAX_INST];
int inst_index = 0;

/*
 * init_inst_table: inst.data 파일로부터 instruction 테이블 초기화
 * 매개변수: filename - instruction 데이터 파일 경로
 * 반환값: 없음
 */
void init_inst_table(char *filename) {
    FILE *fp;
    char line[100];
    char inst_name[10], format_type[5];
    int format;
    unsigned int opcode;

    // 파일 열기
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    inst_index = 0;

    // 파일에서 한 줄씩 읽어서 instruction 테이블에 저장
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 탭으로 구분된 데이터 파싱: 명령어 형식 바이트수 OPCODE
        if (sscanf(line, "%s\t%s\t%d\t%X", inst_name, format_type, &format, &opcode) == 4) {
            // 메모리 할당
            inst_table[inst_index] = (inst *)malloc(sizeof(inst));

            // 데이터 저장
            strcpy(inst_table[inst_index]->str, inst_name);
            inst_table[inst_index]->op = (unsigned char)opcode;
            inst_table[inst_index]->format = format;
            inst_table[inst_index]->ops = format;

            inst_index++;

            // 최대 개수 도달 시 종료
            if (inst_index >= MAX_INST) {
                break;
            }
        }
    }

    fclose(fp);
}

/*
 * search_opcode: instruction 테이블에서 해당 명령어의 OPCODE 검색
 * 매개변수: str - 검색할 instruction 이름
 * 반환값: OPCODE 값 (없으면 -1)
 */
int search_opcode(char *str) {
    int i;

    if (str == NULL) {
        return -1;
    }

    // instruction 테이블 순회하며 검색
    for (i = 0; i < inst_index; i++) {
        if (strcmp(inst_table[i]->str, str) == 0) {
            return inst_table[i]->op;
        }
    }

    // 찾지 못한 경우
    return -1;
}

/*
 * load_input_file: 입력 파일을 읽어 input_data에 저장
 * 매개변수: filename - 어셈블리 소스 파일 경로
 * 반환값: 없음
 */
void load_input_file(char *filename) {
    FILE *fp;
    char line[200];

    // 파일 열기
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    line_num = 0;

    // 파일에서 한 줄씩 읽어서 input_data에 저장
    while (fgets(line, sizeof(line), fp) != NULL && line_num < MAX_LINES) {
        // 줄 끝의 개행 문자 제거
        line[strcspn(line, "\n")] = '\0';

        // 메모리 할당 및 복사
        input_data[line_num] = (char *)malloc(strlen(line) + 1);
        strcpy(input_data[line_num], line);
        line_num++;
    }

    fclose(fp);
}

/*
 * token_parsing: 각 라인을 토큰으로 분리하여 token_table에 저장
 * 반환값: 없음
 */
void token_parsing(void) {
    int i, j;
    char *line_copy;
    char *token_ptr;
    int operand_cnt;

    token_line = 0;

    // 각 입력 처리
    for (i = 0; i < line_num; i++) {
        // 빈 라인이나 주석 라인은 건너뛰기
        if (input_data[i][0] == '\0' || input_data[i][0] == '.') {
            continue;
        }

        // token_table에 메모리 할당
        token_table[token_line] = (token *)malloc(sizeof(token));

        // 초기화
        token_table[token_line]->label = NULL;
        token_table[token_line]->operator = NULL;
        for (j = 0; j < MAX_OPERAND; j++) {
            token_table[token_line]->operand[j][0] = '\0';
        }
        token_table[token_line]->comment[0] = '\0';

        // 라인 복사 (strtok이 원본을 수정하므로)
        line_copy = (char *)malloc(strlen(input_data[i]) + 1);
        strcpy(line_copy, input_data[i]);

        // 주석 처리 먼저 분리
        char *comment_pos = strchr(line_copy, '.');
        if (comment_pos != NULL) {
            strcpy(token_table[token_line]->comment, comment_pos);
            *comment_pos = '\0';  // 주석 부분 제거
        }

        // 첫 번째 문자가 공백이 아니면 label이 있음
        if (line_copy[0] != ' ' && line_copy[0] != '\t' && line_copy[0] != '\0') {
            // Label 파싱
            token_ptr = strtok(line_copy, " \t");
            if (token_ptr != NULL) {
                token_table[token_line]->label = (char *)malloc(strlen(token_ptr) + 1);
                strcpy(token_table[token_line]->label, token_ptr);
                token_ptr = strtok(NULL, " \t");
            }
        } else {
            // 공백으로 시작하면 label 없음
            token_ptr = strtok(line_copy, " \t");
        }

        // Operator 파싱
        if (token_ptr != NULL && token_ptr[0] != '.') {
            token_table[token_line]->operator = (char *)malloc(strlen(token_ptr) + 1);
            strcpy(token_table[token_line]->operator, token_ptr);
        }

        // Operand 파싱 (콤마로 구분)
        token_ptr = strtok(NULL, " \t");
        operand_cnt = 0;

        if (token_ptr != NULL && token_ptr[0] != '.') {
            char *operand_copy = (char *)malloc(strlen(token_ptr) + 1);
            strcpy(operand_copy, token_ptr);

            // 콤마로 구분된 operand 분리
            char *op_token = strtok(operand_copy, ",");
            while (op_token != NULL && operand_cnt < MAX_OPERAND) {
                // 앞쪽 공백 제거
                while (*op_token == ' ' || *op_token == '\t') {
                    op_token++;
                }

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

/*
 * print_output: 토큰 테이블과 OPCODE를 출력
 * 반환값: 없음
 */
void print_output(void) {
    int i, j;
    int opcode;

    // 각 토큰 라인 출력
    for (i = 0; i < token_line; i++) {
        // Label 출력 (8칸)
        if (token_table[i]->label != NULL) {
            printf("%-8s", token_table[i]->label);
        } else {
            printf("        ");  // 8칸 공백
        }

        // Operator 출력 (8칸)
        if (token_table[i]->operator != NULL) {
            printf("%-8s", token_table[i]->operator);
        }

        // Operand 출력 (콤마로 연결)
        for (j = 0; j < MAX_OPERAND; j++) {
            if (token_table[i]->operand[j][0] != '\0') {
                if (j > 0) {
                    printf(",");
                }
                printf("%s", token_table[i]->operand[j]);
            }
        }

        // OPCODE 출력 (16진수 2자리)
        if (token_table[i]->operator != NULL) {
            opcode = search_opcode(token_table[i]->operator);
            if (opcode >= 0) {
                printf("\t\t%02X", opcode);
            }
        }

        printf("\n");
    }
}