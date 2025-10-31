#include "csapp.h"

/*
  typedef struct {
    int rio_fd;                // connfd가 여기 저장됨
    int rio_cnt;               // 버퍼에 남은 바이트
    char *rio_bufptr;          // 다음 읽을 위치
    char rio_buf[8192];        // 내부 버퍼
} rio_t;
*/
void echo(int connfd)
{
  size_t n;          // 읽은 바이트 수
  char buf[MAXLINE]; // 보통 8kb. 클라이언트가 보낸 한 줄을 여기에 저장.
  riot_t rio;        // RIO 구조체 변수. buffered I/O를 위한 상태 정보 저장함.

  Rio_readinitb(&rio, connfd); // 초기화

  // Rio_readlineb: rio의 내부 버퍼에서 한줄(\n까지) 읽고 buf에 복사한 뒤 읽은 바이트 수를 반환한다.
  // 0바이트: 계속 읽음/ 0바이트: EOF/ 0보다 작음: 에러.
  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
  {
    printf("server received %zu bytes\n", n);
    Rio_writen(connfd, buf, n);
  }
}