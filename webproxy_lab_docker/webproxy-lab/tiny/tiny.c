#include "csapp.h"

void doit(int fd);
void read_requestheaders(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize, char *method);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
int append_body(char *buf, int offset, int max_size, char *format, ...);

int main(int argc, char **argv)
{
  // 1. 변수 선언
  int listendfd = 0, connfd = 0;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen; // 기본적으로 최소 32비트 이상의 부호없는 정수. OS별로 다른 정수 크기를 지원함. POSIX 표준임. 각 시스템이 자신에 맞게 정의 할 수 있음 (32bit, 64bit, unsigned int...)
  struct sockaddr_storage clientaddr = {};

  // 2. argc 검사
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  // 3. 리스닝 소켓 생성
  // argv[0]: 프로그램명, argv[1]: port number
  listendfd = Open_listenfd(argv[1]);

  while (1)
  {
    // 4. 연결 큐에서 첫번째 연결 꺼내서 통신용 소켓 생성 (connfd)
    // clientaddr에 클라이언트 IP:port 정보 저장됨. 예) 192.168.1.100:54321
    clientlen = sizeof(clientaddr);
    connfd = Accept(listendfd, (SA *)&clientaddr, &clientlen);

    // 5. 소켓 구조체를 호스트이름과 서비스이름으로 변환 > 클라이언트 정보 받아서 로그 출력
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("✅ line: %d, Accepted connection from (%s, %s)\n", __LINE__, hostname, port);

    // 6.
    doit(connfd);

    // 7. 연결소켓 닫기
    Close(connfd);
  }
}

void doit(int fd)
{
  int is_static = 0;
  struct stat sbuf = {}; // 파일의 메타정보 (크기, 타입, 권한, 시간 등)를 가진 구조체
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio; // buffered I/O 용도 (파일디스크립터, 버퍼에 남은 읽을 바이트, 다음 읽을 위치 포인터, 내부 버퍼)

  // 1. fd연결 및 내부 퍼버 초기화
  Rio_readinitb(&rio, fd);

  // 2. 커널 메모리에서 한 줄만 읽어서 사용자 버퍼로 복사한다
  // buf = "GET /index.html HTTP/1.1\r\n"
  if (!rio_readlineb(&rio, buf, MAXLINE))
    return;
  printf("🐛[request header line 1] >> %s", buf); // 출력 예: GET / HTTP/1.1 또는 GET /cgi-bin/adder?1&2 HTTP/1.1

  // 3. 버퍼를 공백 단위로 잘라서 각 변수에 저장
  // sscanf: 형식에 맞는 데이터를 문자열에서 추출해서 변수에 저장한다.
  // 예 : method = "GET", uri = "/cgi-bin/adder?1&2", version = "HTTP/1.1"
  sscanf(buf, "%s %s %s", method, uri, version);
  printf("🐛 method >> %s ,uri >> %s ,version >> %s\n", method, uri, version);

  // 개발자도구 요청 무시
  if (strcmp(uri, "/.well-known/appspecific/com.chrome.devtools.json") == 0)
  {
    return;
  }

  //  4. Method의 값과 "GET"을 비교
  if (strcasecmp(method, "GET") && strcasecmp(method, "HEAD"))
  {
    clienterror(fd, method, "501", "Not Implemented", "Tiny does not implement this method");
    return;
  }

  // 5. 헤더 나머지를 읽고 버린다.
  read_requestheaders(&rio);

  // 6. uri 파싱
  is_static = parse_uri(uri, filename, cgiargs);
  // printf("🐛 [URI_parse result] static : %d, uri: %s, filename: %s, cgiargs: %s", is_static, uri, filename, cgiargs);

  if (stat(filename, &sbuf) < 0)
  {
    clienterror(fd, filename, "404", "Not found", "Tiny는 이 파일을 찾을 수 없단다..");
    return;
  }

  // 7. 정적 콘텐츠를 요청한 경우
  if (is_static)
  {
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
    {
      clienterror(fd, filename, "403", "Forbiddn", "일반 파일이 아니거나 읽기 권한이 없어서 Tiny는 이 파일 읽을 수 없어요.");
      return;
    }
    serve_static(fd, filename, sbuf.st_size, method);
  }
  else
  {
    // 8. 동적 콘텐츠를 요청한 경우
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
    {
      clienterror(fd, filename, "403", "Forbiddn", "일반 파일이 아니거나 실행 권한이 없어서 Tiny는 이 파일 읽을 수 없어요.");
    }
    serve_dynamic(fd, filename, cgiargs);
  }
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF];
  int offset = 0;

  offset += append_body(body, offset, MAXBUF, "<html><title>Tiny Error</title>");
  offset += append_body(body, offset, MAXBUF, "<body bgcolor=\"ffffff\">\r\n");
  offset += append_body(body, offset, MAXBUF, "%s%s: %s\r\n", body, errnum, shortmsg);
  offset += append_body(body, offset, MAXBUF, "%s<p>%s: %s\r\n", body, longmsg, cause);
  offset += append_body(body, offset, MAXBUF, "%s<hr><em>The Tiny Web Server</em>\r\n", body);

  offset = 0;
  offset += snprintf(buf + offset, MAXLINE - offset, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  offset += snprintf(buf + offset, MAXLINE - offset, "Content-type: text/html; charset=UTF-8\r\n");
  offset += snprintf(buf + offset, MAXLINE - offset, "Content-length: %d\r\n\r\n", (int)strlen(body));

  if (offset > 0 && offset < MAXLINE)
  {
    Rio_writen(fd, buf, offset);
  }
  Rio_writen(fd, body, strlen(body));
}

int append_body(char *buf, int offset, int max_size, char *format, ...)
{
  va_list args;
  va_start(args, format);
  int n = vsnprintf(buf + offset, max_size - offset, format, args);
  va_end(args);

  if (n < 0 || offset + n >= max_size)
  {
    fprintf(stderr, "Buffer overflow\n");
    return -1;
  }
  return n;
}

void read_requestheaders(rio_t *rp)
{
  char buf[MAXLINE];

  // 한 줄을 읽어서 buf에 저장
  Rio_readlineb(rp, buf, MAXLINE);
  while (strcmp(buf, "\r\n")) // 헤더 내용이 끝날 때 까지 출력만 하고 버린다
  {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("🐛request header: %s", buf);
  }
  return;
}

/*
  uri를 filename과 cgiargs로 파싱한다.
  동적 컨텐츠는 0을, 정적 컨텐츠는 1을 리턴한다.
*/

int parse_uri(char *uri, char *filename, char *cgiargs)
{
  // printf("URI: %s\n", &uri);
  char *ptr = NULL;

  // 1. uri에 cig-bin있는지 확인
  if (!strstr(uri, "cgi-bin")) // 없다
  {
    // 2. 정적 콘텐츠 처리
    // http://localhost:8080 (uri = "/")
    strcpy(cgiargs, "");
    // 2.1. 상대 경로로 만들기
    strcpy(filename, "."); // filename = "."
    strcat(filename, uri); // filename = "./"
    // 2.2. 홈이면 자동으로 /home.html 붙여주기
    if (uri[strlen(uri) - 1] == '/')
      strcat(filename, "home.html"); // filename = "./home.html"
    return 1;
  }
  else
  {
    // 3. 동적 콘텐츠 처리
    // http://localhost:8080/cgi-bin/adder?num1=1&num2=2
    ptr = index(uri, '?');
    if (ptr)
    {
      strcpy(cgiargs, ptr + 1); // cgiargs = "num1=1&num2=2"
      *ptr = '\0';              // uri = "http://localhost:8080/cgi-bin/adder\0" .. 널 종료문자로 문자열의 끝을 표시함.
    }
    else
    {
      strcpy(cgiargs, ""); // cgiargs
    }

    strcpy(filename, ".");
    strcat(filename, uri);

    return 0;
  }
}

void serve_static(int fd, char *filename, int filesize, char *method)
{
  int srcfd = 0; // 파일을 읽기 위한 디스크립터
  char filetype[MAXLINE], buf[MAXBUF];
  int offset = 0;

  // 1. 파일의 타입확인
  get_filetype(filename, filetype);

  // 2. response 헤더 만들어서 클라이언트에게 전송
  offset += snprintf(buf + offset, MAXLINE - offset, "HTTP/1.0 200 OK\r\n");
  offset += snprintf(buf + offset, MAXLINE - offset, "Server: Tiny Web Server\r\n");
  offset += snprintf(buf + offset, MAXLINE - offset, "Connection: close\r\n");
  offset += snprintf(buf + offset, MAXLINE - offset, "Content-length: %d\r\n", filesize);
  offset += snprintf(buf + offset, MAXLINE - offset, "Content-type: %s; charset=UTF-8\r\n\r\n", filetype);

  Rio_writen(fd, buf, strlen(buf)); // fd가 클라이언트 소켓이라 TCP/IP 네트워크를 타고 클라이언트까지 전달됨
  printf("🐛Response headers: %s\n", buf);

  // HEAD 요청이면 헤더만 반환하고 종료
  if (strcasecmp(method, "HEAD") == 0)
  {
    return;
  }
  // 문제 11-9. mmap과 rio_readn 대신에 malloc, rio_readn, rio_writen을 사용해서 fd에 복사

  // 전제: 파일을 읽기 위해서는 파일을 open() 하고 파일 내용을 read()해야 함.
  // write()도 마찬가지. 다 끝나면 close()로 파일을 닫는다.

  // 0. Open() : 파일 열기
  srcfd = Open(filename, O_RDONLY, 0);
  // Open()의 흐름
  // 1. 커널에 요청: "filename"을 열어줘.
  // 2. 커널이 파일 전역 객체 생성하고 fd에 객체 포인터를 저장한다
  // 3. fd 반환
  if (srcfd < 0)
  {
    clienterror(fd, filename, "404", "Not found", "Tiny는 이 파일을 찾을 수 없단다..");
    return;
  }

  // 1. 파일 사이즈만큼 동적 메모리 할당 (버퍼 생성)
  char *filedata = (char *)malloc((size_t)filesize);
  // 2. 파일 내용을 버퍼로 복사
  // rio_readn()의 흐름
  // short count방지를 위해 내부적으로 read() 시스템콜을 반복호출한다.
  // 1. srcfd의 내용이 filedata에 filesize만큼 저장된다.
  if (rio_readn(srcfd, filedata, filesize) < 0)
  {
    clienterror(fd, filename, "500", "Internal Server Error", "메모리 할당 실패");
    Close(srcfd);
    Free(filedata);
    return;
  }

  // 3. 버퍼에 저장된 내용을 클라이언트소켓(fd)으로 전송
  // short count방지를 위해 내부적으로 write() 시스템콜을 반복호출한다.
  ssize_t n = rio_writen(fd, filedata, filesize);
  if (n < 0)
  {
    fprintf(stderr, "Rio_writen error: %s\n", strerror(errno));
    Close(srcfd);
    Free(filedata);
    clienterror(fd, filename, "500", "internal server error", "파일 읽기 오류");
    return;
  }

  // 4. 정상 완료. clean up
  Close(srcfd);
  Free(filedata);

  // 2. 파일을 읽기 전용으로 열고 fd를 srcfd에 저장
  // srcfd = Open(filename, O_RDONLY, 0);
  // // 3. srcfd를 filesize만큼 메모리에 매핑
  // srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  // // 4. 파일 디스크립터 닫기
  // Close(srcfd);
  // // 5. srcp에 매핑된 내용을 클라이언트에 전송
  // Rio_writen(fd, srcp, filesize);
  // // 6. 메모리 해제
  // Munmap(srcp, filesize);
}

// CGI프로그램(동적 콘텐츠)를 실행하고 결과를 클라이언트에 보낸다
void serve_dynamic(int fd, char *filename, char *cgiargs)
{
  char buf[MAXLINE], *emptylist[] = {NULL};
  int offset = 0;

  // HTTP응답의 첫 부분을 버퍼에 누적해서 클라이언트에 전송
  offset += snprintf(buf + offset, MAXLINE - offset, "HTTP/1.0 200 OK\r\n");
  offset += snprintf(buf + offset, MAXLINE - offset, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  // 자식 프로세스 생성
  if (Fork() == 0)
  {
    // CGI표준에 따라 쿼리 문자열을 환경변수로 설정 (1 = 덮어쓰기)
    setenv("QUERY_STRING", cgiargs, 1);
    // 자식 프로세스의 표준 출력을 fd로 리다이렉트 (모든 출력이 클라이언트로 전송된다)
    Dup2(fd, STDOUT_FILENO);
    // CGI 프로그램 로드, 실행
    Execve(filename, emptylist, environ);
  }
  // 부모 프로세스는 자식이 완료될 때까지 대기한다
  Wait(NULL);
}

void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".mpg"))
    strcpy(filetype, "video/mpeg");
  else
    strcpy(filetype, "text/plain");
}