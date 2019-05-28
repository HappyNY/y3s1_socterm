Notes
-----

* Instruction Architecture
  * [31:29] 
    ```
    VAL   ID  OPS
    0000  NV  NOP
    0001  AL
    0010  EQ
    0011  NE
    0100  ....
    ```  
  * [28]
    * Status refresh flag 
  * [27:22]
    * OpCodes
    ```
     - Integral ops
    27 26 25:22 ID      OPS
     0  0  0000 MOV
           0001 MVN     Move minus
           0010 ADD
           0011 SUB
           0100 ADC
           0101 SBC     Subtract with carry
           0110 
           0111 
           1000 LSL     Logical shift left
           1001 LSR
           1010 ASR     Arithmetic shift right
           1011 CMP     Compare A == B
           1100 ADI     Add immediate only
           1101 SBI     Subtract immediate only 
           1110 MVI     Load Immediate
           1111 MVNI    Load Immediate minus
     
     - FP ops
    27 26 25:22 ID      OPS
     0  1  0000 NOT
           0001 AND
           0010 OR
           0011 XOR
           0100 
           0101 
           0110 
           0111 
           1000 FCMP    Compare A == B 
           1001 FADD
           1010 FSUB
           1011 FMUL
           1100 FDIV
           1101 FTOI
           1110 ITOF
           1111 FINV
    ```
  * [21: 0]
    * Operands
    ```
    - CASE A 
        21:17   16:12   11:5    4:0
           RD      RB     IM     RA
    - CASE B
        21:17   16:12   11:0
           RD      RB     IM
    - CASE C
        21:17   16: 0
           RD      IM
    ```
* Speed up clocks
  * 파이프라이닝 도입.
  * 데이터 해저드 이슈
  * 레지스터 수 up, 태스크 병렬 실행
  * 즉, 태스크 A B C D에 대하여
    * A0 B0 C0 D0 A1 B1 C1 D1 ...
    * 명령어를 뒤섞는다.
    * 각 태스크에 할당되는 레지스터 = 8개
    * 각 태스크의 마지막 레지스터 = 태스크 오프셋 레지스터
    * 어셈블리는 아래의 꼴에서 
    ```assembly
        movc r7, 0
        ld r0, [r7]
        ld r1, [r7 + #4]
        ld r2, [r7 + #8] ;task 1
        addc r7, r7, #12
        ld r0, [r7]
        ld r1, [r7 + #4]
        ld r2, [r7 + #8] ;task 2
    ```
    * 아래와 같은 꼴로 표현된다
    ```assembly
        movc r7, 0
        movc r15, 12
        movc r23, 24
        movc r31, 36
        ld r0, [r7]
        ld r8, [r15]
        ld r16, [r23]
        ld r24, [r31]
        ...
    ```
    * 이 방법으로, 파이프라인 스테이지를 안전하게 늘릴 수 있음.
    * 각 태스크당 할당되는 레지스터의 수가 적긴 하지만, 이는 SRAM을 사용하여 지연이 없는 로컬 메모리 및, 상수 오프셋 참조가 자유로운  어느정도 극복 가능.
    * 