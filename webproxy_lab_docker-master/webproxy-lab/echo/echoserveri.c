#include "csapp.h"

int main(int argc, char **argv)
{
  int listenfd, connfd;               // 서버 리슨용 소켓과 클라이언트 연결용 소켓
  socklen_t clientlen;                // 클라이언트 주소 구조체의 크기
  struct sockaddr_storage clientaddr; // 클라이언트 주소 정보를 저장할 구조체
  char client_hostname[MAXLINE];      // 클라이언트 호스트 이름 문자열
  char client_port[MAXLINE];          // 클라이언트 포트 번호 문자열

  // 명령줄 인자가 2개가 아니면 사용법 출력 후 종료
  if (argc != 2)
  {
    fprintf(stderr, "usage")
  }
}