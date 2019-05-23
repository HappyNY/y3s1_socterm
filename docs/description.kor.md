System description 
=====================================

## Introduce
- This system supports complicated floating point operations on 3D rendering jobs.
- These are described as simple threads which supports deterministic programs only.
- Memory spaces are like below:
  - Readonly constant space(cached)
  - Writeonly upload stream
  - Local registers

## Implementation blueprint
```verilog
    module System
        // On the same bus lanes
        module CPU Memory
        module Processing System
        module GPU Memory
        module GPGPU
            generate n
                module Core Group
                    module Instr Cache
                    module Upload Buffer
                    module Data feeder // read from gmem
                    module RegBankSFR // 16..31
                    generate n * 16
                        module RegBank // 1..15
                        module ALU
                    endgenerate
                endmodule
            endgenerate
                        
            
        
```

## Features
> 클럭은 AHB 버스와 동기화시킴!
- 코어는 반드시 데이터 배열을 나타낸다.
  - 쓰레드는 반드시 하나의 구조체 영역을 나타낸다.
  - 즉, 하나의 코어는 16개의 구조체 집합과 같다.
  - 모든 프로그램은 코어의 시작 주소 지점으로부터, 각 구조체의 오프셋에 대한 상대 주소를 바탕으로 실행된다.
  - 전자는 Offset, 후자는 Interval.
  - 하나의 코어는 C의 배열과 같은 개념으로 계산을 수행.
  - 하나의 프로세스는 아래와 동치이다.
    ```C
        struct Thread_t {
            char v[Interval];
        };
        
        void Process() {
            Thread_t Core[16];
            // &Core[0] == Offset
            for(int i = 0; i < 16; ++i) {
                DoSomeWork(&Core[i]);
            }
        }
    ```
        
- 병렬 계산
  - 한 개의 Core가 열 여섯개의 Thread를 관리
  - 1~15번 레지스터는 지역 레지스터.
    - [1~15] <- [1~15] 형태의 오퍼레이션은 지역 레지스터 간 데이터 교환, 산술 연산이 가능
    - 16~31번 레지스터는 Core가 갖는 공용 레지스터
    - [1~15] <- [16~31] 형태의 오퍼레이션은 빠르게 지역 레지스터에 상수를 로드할 수 있게끔 한다.
    - [16~31] <- [1~15] 형태의 연산은 정의 x
- 전역 메모리 로드
  - [16~31] <- gmem 형태의 연산이 정의된다
  - 이를 이용, 행렬 등의 데이터를 빠르게 로드 가능

## Process
- CPU는 가장 먼저, 실행할 프로그램을 코어의 인스트럭션 캐시에 로드한다. 인스트럭션 캐시의 크기는 4KByte로 제한된다.
- 그 뒤, 별도의 포트를 통해 디바이스의 R30을 설정한다
  - R30에는 코어의 오프셋과 각 쓰레드(구조체)의 인터벌이 4byte 단위로 기록된다.
  - R30의 MSB가 1로 세트되면 디바이스의 작업이 시작된다.
- 프로그램은 상수 시간에 실행된다. 조건문 미지원
  - 가장 먼저, Fetcher가 인스트럭션을 로드한다
  - 만약 공용 상수 데이터(R16~29가 Destination)가 필요한 경우, 단일 명령으로 데이터를 로드한다.
  - 만약 지역 상수 데이터(R0~R15가 Destination)이 필요한 경우, 

## Instrs
- **ldl rd, [ra], rb**
  - Load from memory by local offset
  - rd = cmem[(ra+rb/4)*4]
- **stl  [rd], ra, rb**
  - Store to memory by local offset
- **movc rd, ra** 
  - <u>Must be between r16-31</u>
  - Move common register to all local registers
- **mov rd, ra**
  - Move between local registers
- **Arithmatics**
  - **add**
  - **itof**
  - **ftoi**
  - **mul**
  - **reci** 
    - gets reciprocal
## Registers
- R0
  - Hard-wired 0
- R1~R14
  - 32bit general perpose registers
- R15
  - Hard-wired thread begin address.
  - **Means a thread is treated as a structure.**
- R16-R31 (Special function registers(Common per core groups))
  - R16~R29: 
  - R30: Control register, takes data from external port
    - [7 ..0]  Step, 4 byte in units. 
    - [30..8] Offset, 4 byte in units.
    - [31] Core enable. Resets PC on clear 
  - R31: PC
