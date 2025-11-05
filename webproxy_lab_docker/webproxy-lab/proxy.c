#include "csapp.h"

/* 과제 조건: HTTP/1.0 GET 요청을 처리하는 기본 sequential proxy

  클라이언트의 요청 (to proxy)
  → proxy
  - URI 파싱
  - 웹서버에 대신 요청을 전달
  - 웹 서버와 연결
  - 서버의 응답을 클라이언트에 전달
*/

/* 프록시 캐시의 최대 크기 */
#define MAX_CACHE_SIZE 1049000
/* 캐시에 저장할 수 있는 개별 웹 오브젝트의 최대 크기 */
#define MAX_OBJECT_SIZE 102400

/* 프록시가 웹 서버에 보낼 자신에 대한 정보 */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";
static const char *new_version = "HTTP/1.0";

void do_it(int fd);
void do_request(int clientfd, char *method, char *uri_ptos, char *host);
void do_response(int connfd, int clientfd);
int parse_uri(char *uri, char *uri_ptos, char *host, char *port);
void *thread(void *vargp); // vargp: void argument pointer

/* 연결 및 병행성 관리 */
int main(int argc, char **argv)
{
  // #region 변수 선언부
  int listenfd, *connfdp;
  char hostname[MAXLINE], port[MAXLINE];

  /* 소켓주소 구조체의 길이를 나타내는 전용데이터 타입. 부호없는 32비트 이상의 정수 */
  socklen_t clientlen;
  /* 범용 소켓 주소 구조체 */
  struct sockaddr_storage cliendtaddr;
  /* 스레드를 식별하기 위한 데이터 타입 */
  pthread_t tid;
  // #endregion

  // #region 포트번호 넣었는지 확인
  /* 프로그램이 올바르게 실행되었는지 확인. 예) ./proxy 8080 ..사용자가 포트번호 빼먹고 실행할 수도 있어서 확인한다 */
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    /* 프로그램 완전 종료. exit(0): 정상 종료, exit(1): 에러로 인한 종료 */
    exit(1);
  }
  // #endregion

  /*================== 👷 1. 리스닝 소켓 만들기 ==================*/
  /* 포트번호에 해당하는 리스닝 소켓 식별자를 열어준다 */
  listenfd = Open_listenfd(argv[1]);

  /* 클라이언트의 요청이 올 때마다 새로운 커넥션 소켓을 만들고 doit() 호출 */
  while (1)
  {
    /*================== 👷 2. 커넥션 소켓 만들기 ==================*/
    /* 멀티스레딩 환경에서는 각 스레드가 독립적인 소켓을 가져야 해서 malloc이 필수 */
    connfdp = Malloc(sizeof(int));
    /* 클라이언트에게서 받은 연결 요청을 accept하고 개별 커넥션 소켓 생성 */
    clientlen = sizeof(cliendtaddr);
    *connfdp = Accept(listenfd, (SA *)&cliendtaddr, &clientlen);

    // #region 연결 성공 메세지 출력
    Getnameinfo((SA *)&cliendtaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("✅ Accepted connection from (%s, %s)\n", hostname, port);
    // #endregion

    /*================== 👷 3.스레드 생성 ==================*/
    Pthread_create(&tid, NULL, thread, connfdp); // 생성된 스레드를 저장할 포인터, 스레드 속성 설정, 스레드 생성 후 실행할 함수, 함수에 전달할 인자
  }
}

/* per client thread logic */
void *thread(void *vargp)
{
  /* 클라이언트와 통신할 소켓 */
  int connfd = *((int *)vargp);
  /* 스레드를 분리 상태로 설정해서 스레드 종료후 자동으로 리소스가 회수되도록 한다 */
  Pthread_detach(pthread_self());
  /* Malloc()으로 할당한 메모리 해제 */
  Free(vargp); // Phread_create 스레드는 자신의 리소스만 관리하고, 동적 할된 메모리는 자동해제하지 않으므로 명시적으로 해제해야 함

  do_it(connfd);
  Close(connfd);
  return NULL;
}

/* Core client-proxy-server transaction */
void do_it(int connfd)
{
  // #region 변수 선언부
  int clientfd;
  char buf[MAXLINE], host[MAXLINE], port[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char uri_ptos[MAXLINE];
  rio_t rio;
  // #endregion

  /*================== 👷 1.host,port,path 추출 ==================*/

  /* rio 버퍼와 fd(proxy의 connfd)를 연결시켜준다 */
  Rio_readinitb(&rio, connfd);
  /* rio에있는 내용을 모두 buf로 옮긴다 */
  Rio_readlineb(&rio, buf, MAXLINE);

  printf("🐛 Request headers to proxy:\n");
  printf("%s\n", buf);

  /* buf에서 문자열을 읽어와서 각 변수에 저장 */
  // 🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨 디버깅 중
  sscanf(buf, "%s %s %s", method, uri, version);
  int result = parse_uri(uri, uri_ptos, host, port);
  printf("🐛 parse result: %d", result);

  /* server의 리스닝 소켓 연결 */
  clientfd = Open_clientfd(host, port);
  printf("🐛 Open_clientfd() clientfd: %d\n", clientfd);

  /*===========👷 2. 클라이언트의 요청 읽고 서버에 전달 =========*/
  do_request(clientfd, method, uri_ptos, host);

  /*===========👷 3. 서버의 응답을 클라이언트에 전달 =========*/
  do_response(connfd, clientfd);

  /* 리스닝 소켓 연결 종료 */
  Close(clientfd);
}

/* proxy => server */
void do_request(int clientfd, char *method, char *uri_ptos, char *host)
{
  char buf[MAXLINE];
  printf("🐛 Request headers to server: \n");
  printf("%s %s %s\n", method, uri_ptos, new_version);

  /*================== 👷 1. 요청 헤더 읽고 요청 구조화==================*/
  int offset = 0;
  // GET /index.html HTTP/1.0
  offset += snprintf(buf + offset, MAXLINE - offset, "GET %s %s\r\n", uri_ptos, new_version);
  // Host: www.google.com
  offset += snprintf(buf + offset, MAXLINE - offset, "Host: %s\r\n", host);
  // User-Agent: ~(bla bla)
  offset += snprintf(buf + offset, MAXLINE - offset, "%s", user_agent_hdr);
  // Connections: close
  offset += snprintf(buf + offset, MAXLINE - offset, "Connection: cloes\r\n");
  // Proxy-Connection: close
  offset += snprintf(buf + offset, MAXLINE - offset, "Proxy-Connection: close\r\n\r\n");

  /*============= 👷 2. 구조화한 요청을 서버로 전송 =============*/
  Rio_writen(clientfd, buf, (size_t)strlen(buf));
}

/* server => proxy */
void do_response(int connfd, int clientfd)
{
  char buf[MAX_CACHE_SIZE];
  ssize_t n;
  rio_t rio;

  /*============= 👷 1. 서버로부터 온 응답 읽기 =============*/
  Rio_readinitb(&rio, clientfd);
  n = Rio_readnb(&rio, buf, MAX_CACHE_SIZE);

  /*============= 👷 2. 응답을 클라이언트로 보내줌 =============*/
  Rio_writen(connfd, buf, n);
}

int parse_uri(char *uri, char *uri_ptos, char *host, char *port)
{
  char *ptr;

  /*============= 👷 1. URI에서 필요한 데이터 추출 =============*/
  printf("👷 1. URI에서 필요한 데이터 추출\n");
  /* http:// 잘라서 host 추출 */
  if (!(ptr = strstr(uri, "://")))
    return -1; // ://가 없으면 invalid uri
  ptr += 3;
  strcpy(host, ptr); // host = www.google.com:80/index.html
  printf("🐛 host: %s\n", host);

  /* uri_ptos(proxy => server로 보낼 uri) 추출 */
  if ((ptr = strstr(host, "/")))
  {
    *ptr = '\0'; // host = www.google.com:80
    ptr += 1;
    strcpy(uri_ptos, "/"); // uri_ptos = /
    strcpy(uri_ptos, ptr); // uri_ptos = /index.html
  }
  else
  {
    strcpy(uri_ptos, "/");
  }
  printf("🐛 uri_ptos: %s\n", uri_ptos);

  /* port 추출 */
  if ((ptr = strstr(host, ":")))
  {              // host = www.google.com:80
    *ptr = '\0'; // host = www.google.com
    ptr += 1;
    strcpy(port, ptr); // port = 80
  }
  else
  {
    strcpy(port, "80"); // port 없을 경우 "80"을 넣어줌
  }
  printf("🐛 port: %s\n", port);

  /*
  Parsing 전. Client로부터 받은 Request Line
  => GET http://www.google.com:80/index.html HTTP/1.1

  Pasirng 후.
  => host = www.google.com
  => uri_ptos = /index.html
  => port = 80
  => Server로 보낼 Request Line
  => GET /index.html HTTP/1.1
  */

  return 0; // return for valid check
}