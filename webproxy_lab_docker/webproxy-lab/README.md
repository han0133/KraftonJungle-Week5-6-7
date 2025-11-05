####################################################################
# CS:APP Proxy Lab
#
# Student Source Files
####################################################################

This directory contains the files you will need for the CS:APP Proxy
Lab.

proxy.c
csapp.h
csapp.c
    These are starter files.  csapp.c and csapp.h are described in
    your textbook. 

    You may make any changes you like to these files.  And you may
    create and handin any additional files you like.

    Please use `port-for-user.pl' or 'free-port.sh' to generate
    unique ports for your proxy or tiny server. 

Makefile
    This is the makefile that builds the proxy program.  Type "make"
    to build your solution, or "make clean" followed by "make" for a
    fresh build. 

    Type "make handin" to create the tarfile that you will be handing
    in. You can modify it any way you like. Your instructor will use your
    Makefile to build your proxy from source.

port-for-user.pl
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>

free-port.sh
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

driver.sh
    The autograder for Basic, Concurrency, and Cache.        
    usage: ./driver.sh

nop-server.py
     helper for the autograder.         

tiny
    Tiny Web server from the CS:APP text


####################################################################
# CS:APP Proxy Lab 한글 번역 2025.11.05 sol
#
# 문제
####################################################################

# 과제 개요
이 과제는 HTTP 캐싱 웹 프록시를 작성하는 프로젝트입니다.
웹 프록시는 웹 브라우저와 최종 서버 사이의 중개자 역할을 하는 프로그램으로, 
브라우저의 요청을 받아 서버에 전달하고, 서버의 응답을 다시 브라우저에 전달합니다.

## Part I: 순차 웹 프록시 구현

첫 번째 부분에서는 HTTP/1.0 GET 요청을 처리하는 기본 프록시를 만듭니다. 

프록시는 다음의 기능을 수행해야 합니다:​
- 명령줄에서 지정된 포트 번호로 들어오는 연결을 수신합니다
- 클라이언트의 요청을 읽고 파싱합니다
- 적절한 웹 서버에 연결을 설정합니다
- 요청한 객체를 서버에 요청합니다
- 서버의 응답을 읽고 클라이언트에 전달합니다

**HTTP 요청 처리**

프록시는 요청 라인의 호스트명과 경로를 파싱해야 하며, 
HTTP/1.1 요청을 받으면 HTTP/1.0 형식으로 변환하여 전달합니다

- 브라우저에서 보낸 요청
`GET http://www.cmu.edu/hub/index.html HTTP/1.1`
- 프록시가 서버에 보내는 요청
`GET /hub/index.html HTTP/1.0`

**프록시가 항상 포함해야 하는 헤더들**
- Host 헤더: 최종 서버의 호스트명 (예: `Host: www.cmu.edu`)
- User-Agent 헤더: `Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3`
- Connection 헤더: `Connection: close`
- Proxy-Connection 헤더: `Proxy-Connection: close`


## Part II: 동시성 처리
프록시를 수정하여 여러 요청을 동시에 처리하도록 업그레이드합니다. 
각 새로운 연결마다 새로운 스레드를 생성하는 방식으로 구현하며, 
스레드는 **분리 모드(detached mode)**에서 실행되어야 메모리 누수를 방지할 수 있습니다.


## Part III: 웹 객체 캐싱
마지막 부분에서는 프록시에 메모리 캐시 기능을 추가합니다.​

**캐시 크기 제한**
- 최대 캐시 크기: 1 MiB (메타데이터 제외)
- 최대 객체 크기: 100 KiB (이를 초과하는 객체는 캐시하지 않음)

**제거 정책**

캐시가 가득 차면 LRU(Least Recently Used) 정책을 근사한 방식으로 객체를 제거합니다. 읽기와 쓰기 모두 *사용* 으로 간주됩니다

**동기화**

캐시 접근은 스레드 안전해야 하며, 특히 여러 스레드가 동시에 캐시에서 읽을 수 있어야 합니다. 단, 쓰기는 한 번에 한 스레드만 가능합니다. 하나의 큰 배타적 락으로 보호하는 것은 허용되지 않으며, 캐시 분할이나 Pthreads 읽기-쓰기 락(reader-writer lock) 사용을 고려해야 합니다.

**평가 기준** 

총 70점:​
- 기본 기능: 40점
- 동시성: 15점
- 캐싱: 15점


## 중요 구현 노트

**프록시 구현 시 고려할 사항들:​**

- 소켓 입출력에는 표준 I/O 함수 대신 RIO(Robust I/O) 패키지 사용
- SIGPIPE 신호 무시 및 EPIPE 오류 처리
- 바이너리 데이터 처리 (이미지, 비디오 등)
- 조기에 닫힌 소켓에서 ECONNRESET 오류 처리
- 모든 요청을 HTTP/1.0으로 전달
- 장시간 실행되는 서버이므로 오류 발생 시 즉시 종료하면 안 됨