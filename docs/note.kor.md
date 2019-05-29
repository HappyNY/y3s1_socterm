Notes
-----

* Instruction Architecture
  * All Instr
    ```
    -- CONDITIONS   [3:0]
    EQ      ZERO FLAG SET 
    NE      NOT ZERO
    OV      OVERFLOW
    NO      NO OVERFLOW
    CY      CARRY SET
    NC      CARRY UNSET
    NE      NEGATIVE(SIGN BIT SET)
    PO      POSITIVE(SIGN BIT UNSET)
    
    -- OPRERANDS    [1:0]
    REG     REG DST, A, B, IMM0
    IM1     REG DST, B, IMM1
    IMA     REG DST, IMMEDIATE ALL
    IMB     REG A, IMMEIDATE ALL (FOR STORING CONSTANT)
    
    -- STATUS REFRESH FLAG
    S       1 BIT
    
    -- ARTIMETICS   [4:0]
    [4:3] == 2'B11 => NO WRITEBACK
    0.   MOV    D :=  B
    1.   INV    D := ~B +    1
    2.   NOT    D := ~B
    3.   ADD    D :=  A +    B
    4.   ADC    D :=  A +    B +    C
    5.   SBC    D :=  A +   ~B +   ~C
    6.   LSL    D :=  A <<   B
    7.   LSR    D :=  A >>   B
    8.   ASL    D :=  A >>>  B
    9.   AND    D :=  A &    B  
    10.  ORR    D :=  A |    B
    11.  XOR    D :=  A ^    B

    12.  CMP    C :=  A == B

    16.  ITOF   INTEGER TO FLOAT
    17.  FTOI   FLOAT TO INTEGER
    18.  FMUL   FLOAT MULTIPLY
    19.  FDIV   FLOAT 
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