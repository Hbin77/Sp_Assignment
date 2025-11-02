// #include <stdio.h>
// #include <string.h>
//
// // 전역 변수 (레지스터 값)
// int PC = 0x003000;     // 프로그램 카운터
// int BASE = 0x006000;   // 베이스 레지스터
// int X_REG = 0x000090;  // 인덱스 레지스터
// int memory[0x100000];  // 1MB 메모리
//
// // 16진수를 10진수 정수로 변환
// int hex_to_int(char *hex) {
//     int result;
//     sscanf(hex, "%x", &result);
//     return result;
// }
//
// // 16진수를 2진수로 변환
// void hex_to_binary(char *hex, char *binary) {
//     int i, val;
//     binary[0] = '\0';
//
//     for (i = 0; hex[i] != '\0'; i++) {
//         if (hex[i] >= '0' && hex[i] <= '9')
//             val = hex[i] - '0';
//         else if (hex[i] >= 'A' && hex[i] <= 'F')
//             val = hex[i] - 'A' + 10;
//         else if (hex[i] >= 'a' && hex[i] <= 'f')
//             val = hex[i] - 'a' + 10;
//         else
//             continue;
//
//         // 각 16진수를 4비트 2진수로
//         strcat(binary, (val & 8) ? "1" : "0");
//         strcat(binary, (val & 4) ? "1" : "0");
//         strcat(binary, (val & 2) ? "1" : "0");
//         strcat(binary, (val & 1) ? "1" : "0");
//     }
// }
//
// // 부호 확장 (12비트를 24비트로)
// int sign_extend_12(int value) {
//     if (value & 0x800) // 12번째 비트가 1이면 음수
//         return value | 0xFFFFF000;
//     return value;
// }
//
// // SIC/XE 명령어 분석 함수
// void analyze_instruction(char *hex_code) {
//     char binary[100];
//     int instruction, len;
//     int opcode, n, i, x, b, p, e, disp_addr;
//     int format, target_addr, reg_a_value;
//
//     //16진수를 정수와 2진수로 변환
//     instruction = hex_to_int(hex_code);
//     hex_to_binary(hex_code, binary);
//     len = strlen(hex_code);
//
//     // 형식 판별 (6자리=Format3, 8자리=Format4)
//     format = (len == 6) ? 3 : 4;
//
//     // 필드 추출
//     if (format == 3) {
//         opcode = (instruction >> 16) & 0xFC;  // 상위 8비트 중 6비트
//         n = (instruction >> 17) & 1;
//         i = (instruction >> 16) & 1;
//         x = (instruction >> 15) & 1;
//         b = (instruction >> 14) & 1;
//         p = (instruction >> 13) & 1;
//         e = (instruction >> 12) & 1;
//         disp_addr = instruction & 0xFFF;      // 12비트
//     } else {
//         opcode = (instruction >> 24) & 0xFC;
//         n = (instruction >> 25) & 1;
//         i = (instruction >> 24) & 1;
//         x = (instruction >> 23) & 1;
//         b = (instruction >> 22) & 1;
//         p = (instruction >> 21) & 1;
//         e = (instruction >> 20) & 1;
//         disp_addr = instruction & 0xFFFFF;    // 20비트
//     }
//
//     // 주소 지정 방식 결정 및 TA 계산
//     char addr_mode[100] = "";
//
//     // n, i 비트로 주소 지정 타입 결정
//     if (n == 1 && i == 1)
//         strcpy(addr_mode, "Simple");
//     else if (n == 1 && i == 0)
//         strcpy(addr_mode, "Indirect");
//     else if (n == 0 && i == 1)
//         strcpy(addr_mode, "Immediate");
//     else
//         strcpy(addr_mode, "SIC");
//
//     // b, p, e로 상대 주소 방식 결정 및 TA 계산
//     if (n == 0 && i == 1) {
//         // Immediate: TA는 값 자체
//         target_addr = disp_addr;
//         strcat(addr_mode, ", immediate");
//     }else if (n == 0 && i == 0) {
//         target_addr = instruction & 0x7FFF;
//         strcat(addr_mode, ", direct (15-bit SIC)");
//     } else if (e == 1) {
//         // Format 4: direct addressing
//         target_addr = disp_addr;
//         strcat(addr_mode, ", extended");
//     } else if (p == 1 && b == 0) {
//         // PC-relative
//         target_addr = PC + sign_extend_12(disp_addr);
//         strcat(addr_mode, ", PC-relative");
//     } else if (p == 0 && b == 1) {
//         // Base-relative
//         target_addr = BASE + disp_addr;
//         strcat(addr_mode, ", base-relative");
//     } else {
//         // Direct
//         target_addr = disp_addr;
//         strcat(addr_mode, ", direct");
//     }
//
//     // Indexed addressing 추가
//     if (x == 1) {
//         target_addr += X_REG;
//         strcat(addr_mode, ", indexed");
//     }
//
//     // 레지스터 A 값 결정
//     if (n == 0 && i == 1) {
//         // Immediate: 값 자체
//         reg_a_value = target_addr;
//     } else if (n == 1 && i == 0) {
//         // Indirect: TA가 가리키는 곳의 값을 주소로 사용
//         int indirect_addr = memory[target_addr];
//         reg_a_value = memory[indirect_addr];
//     } else {
//         // Simple: TA가 가리키는 메모리의 값
//         reg_a_value = memory[target_addr];
//     }
//
//     // 결과 출력
//     printf("\n========= SIC/XE 명령어 분석 =========\n");
//     printf("Hex 입력: %s\n\n", hex_code);
//     printf("1. Binary code:  %s\n", binary);
//     printf("2. Opcode:       %02X\n", opcode);
//     printf("3. Flag bits:    n=%d i=%d x=%d b=%d p=%d e=%d\n", n, i, x, b, p, e);
//     printf("4. Address mode: %s, Format %d\n", addr_mode, format);
//
//     // Disp/Addr 출력
//     if (format == 3) {
//         if (n == 0 && i == 0) {
//             int sic_addr = (instruction & 0x7FFF);  // 하위 15비트
//             printf("5. Disp/Addr:    %04X (15-bit, SIC mode)\n", sic_addr);
//         } else {
//             printf("5. Disp/Addr:    %03X (12-bit)\n", disp_addr);
//         }
//     } else {
//         printf("5. Disp/Addr:    %05X (20-bit)\n", disp_addr);
//     }
//
//     printf("6. Target Address:     0x%04X\n", target_addr);
//     printf("7. Register A value:   0x%06X\n", reg_a_value);
//     printf("===================================\n");
// }
//
// int main() {
//     printf("목적 코드 생성을 위한 I/O 파서 구현\n");
//     printf("==========================\n\n");
//
//     // 메모리 초기화
//     memory[0x3030] = 0x003600;
//     memory[0x3600] = 0x103000;
//     memory[0x6390] = 0x00C303;
//     memory[0xC303] = 0x003030;
//
//     // 강의 속 케이스
//     printf("PC = 0x%06X, BASE = 0x%06X, X = 0x%06X\n", PC, BASE, X_REG);
//
//     char test_cases[6][20] = {
//         "032600",    // Simple, PC-relative
//         "03C300",    // Simple, indexed, base-relative
//         "022030",    // Indirect, PC-relative
//         "010030",    // Immediate
//         "003600",    // SIC direct
//         "0310C303"   // Simple, extended (Format 4)
//     };
//
//     for (int i = 0; i < 6; i++) {
//         analyze_instruction(test_cases[i]);
//     }
//
//     // 사용자가 직접 입력
//     char input[20];
//     while (1) {
//         printf("\nHex 입력 (나가려면 'exit'): ");
//         scanf("%s", input);
//         if (strcmp(input, "exit") == 0) break;
//         analyze_instruction(input);
//     }
//
//     return 0;
// }