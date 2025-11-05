#include "../csapp.h"

int main(void)
{
  char *buf, *p;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1 = 0, n2 = 0;

  if ((buf = getenv("QUERY_STRING")) != NULL)
  {
    // num1=1&num2=2
    p = strchr(buf, '&'); // 특정 문자의 첫번째 등장 위치를 찾아줌
    if (p != NULL)
    {
      // 파라미터 분리
      *p = '\0';
      strcpy(arg1, buf);
      strcpy(arg2, p + 1);

      // sscanf()로 값 추출
      sscanf(arg1, "num1=%d", &n1);
      sscanf(arg2, "num2=%d", &n2);
    }
  }

  // 동적으로 HTML 생성
  // 지정된 형식에 따라 가변 인수를 처리하여 결과를 문자열 버퍼에 기록한다.
  snprintf(content, MAXLINE, "Welcome!<br>"
                             "<p> The answer is : % d + % d = % d\r\n</p> <br>"
                             "<p><a href=\"/\">뒤로가기</a></p>\r\n",
           n1, n2, n1 + n2);

  // HTTP 응답 헤더 출력
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html; charset= UTF-8\r\n\r\n");

  // HTTP 응답 본문 출력
  printf("%s", content);
  fflush(stdout);

  return 0;
}