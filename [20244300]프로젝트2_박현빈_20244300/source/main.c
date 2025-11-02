/*
* main.c
 * 시스템 프로그래밍 프로젝트 #2
 * 메인 프로그램
 */

#include "my_assembler.h"

/*
 * main: 프로그램 진입점
 * 매개변수: argc - 명령행 인자 개수
 *          argv - 명령행 인자 배열
 * 반환값: 0 (정상 종료)
 */
int main(int argc, char *argv[]) {
    char *inst_file = "inst.data";      // instruction 데이터 파일
    char *input_file = "input.asm";     // 어셈블리 소스 파일

    // 명령행 인자 처리
    if (argc >= 3) {
        inst_file = argv[1];
        input_file = argv[2];
    }

    // 1단계: Instruction 테이블 초기화
    init_inst_table(inst_file);

    // 2단계: 입력 파일 로드
    load_input_file(input_file);

    // 3단계: 토큰 파싱
    token_parsing();

    // 4단계: 결과 출력
    print_output();

    return 0;
}