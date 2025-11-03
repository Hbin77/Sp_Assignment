#include "my_assembler.h"

int main(int argc, char *argv[]) {
    char *inst_file = "inst.data";
    char *input_file = "input.asm";

    // 명령행 인자 처리
    if (argc >= 3) {
        inst_file = argv[1];
        input_file = argv[2];
    }

    // 1단계: instruction 테이블 초기화
    init_inst_table(inst_file);

    // 2단계: 입력 파일 읽기
    load_input_file(input_file);

    // 3단계: 토큰 분리
    token_parsing();

    // 4단계: 결과 출력
    print_output();

    return 0;
}