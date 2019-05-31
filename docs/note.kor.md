Notes
-----

* Instruction Set Architecture
  * All Instr
    ```
    -- CONDITIONS   [3:0]
    0.  NE      NOT ZERO
    1.  EQ      ZERO FLAG SET 
    2.  OV      OVERFLOW
    3.  CY      CARRY SET
    4.  NC      CARRY UNSET
    5.  NE      NEGATIVE(SIGN BIT SET)
    6.  PO      POSITIVE(SIGN BIT UNSET)
    7.  AL      ALWAYS ( DEFAULT )
    
    -- STATUS REFRESH FLAG (Only effective on arithmetic operations)
    S       1 BIT
    
    -- OPERAND MODES
        -  D Destination Reg
        -  A Register A
        -  I Immediate
        -  B Register B

       - MODE 1
         -  DDDDD AAAAA II_IIIII BBBBB
         -  Reg A, B, Dst. 
            -  If Arithmetic opr = B will deleivered as B << I // If negative, LSR, MSB = Arithmetic vs Logical
            -  If LD/ST -> 
       - MODE 2
         -  DDDDD AAAAA II_IIIII_IIIII
         -  REG A, DST, IMMED[9:0]
       - MODE 3
         -  DDDDD II_IIIII_IIIII_IIIII
         -  REG DST, IMMED[14:0]
            
    -- OPRS   [4:0]
    - M := B << I0
    VAL  ID     MOD OPR         DESC
    1.   MOV    1   D := M      CAN BE LSL AT THE SAME TIME
    2.   MVN    1   D := 'M+1
    3.   ADC    1   D := A+M+C
    4.   SBC    1   D := A+'M+'C
    5.   AND    1   D := A&M
    6.   ORR    1   D := A|M
    7.   XOR    1   D := A^M
    8.   ADI    2   D := A+I
    9.   SBI    2   D := A-I
    10.  MVI    3   D := I
    11.  ITOF   1               INTEGER TO FLOAT
    12.  FTOI   1               FLOAT TO INTEGER
    13.  FMUL   1               FLOAT MULTIPLY
    14.  FDIV   1               FLOAT DIVISION
    15.  FADD   1               FLOAT ADDITION 
    16.  FSUB   1               FLOAT SUBTRACTION
    17.  FNEG   1               FLOAT NEGATION
    18.  .
    19.  .
    20.  LDL    2   D := L[A+I], @todo. Add mux to input address for local memory. 
    21.  LDC    2   D := G[A+I]
    22.  LDCI   3   D := G[I]
    23.  .
    24.  STL    1   L[B+I] := A
    25.  
    ```
* Speed up clocks
  * 파이프라이닝 도입.
  * 데이터 해저드 이슈
  * 레지스터 수 up, 태스크 병렬 실행
  * 즉, 태스크 A B C D에 대하여
    * 명령어 수준의 병렬성을 부여
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
        mvi r7, 0
        mvi r15, 12
        mvi r23, 24
        mvi r31, 36
        ld r0, [r7]
        ld r8, [r15]
        ld r16, [r23]
        ld r24, [r31]
        ...
    ```
    * 이 방법으로, 파이프라인 스테이지를 안전하게 늘릴 수 있음.
    * 각 태스크당 할당되는 레지스터의 수가 적긴 하지만, 이는 SRAM을 사용하여 지연이 없는 로컬 메모리 및, 상수 오프셋 참조가 자유로운  어느정도 극복 가능.
    * 