#include <stdio.h>
#include <stdbool.h>

// 기본 연산 함수
static inline size_t align_size(size_t size);
static inline void update_value(void *ptr, unsigned long val);
static inline unsigned long get_value(void *ptr);

// 블록 정보 추출 함수
static inline size_t get_size(void *header_ptr);
static inline bool is_allocated(void *header_ptr);

// 블록 포인터 계산 함수
static inline void *get_header_pointer(void *pPayload);
static inline void *get_footer_pointer(void *pPayload);
static inline void *get_next_payload_pointer(void *pPayload);
static inline void *get_prev_payload_pointer(void *pPayload);

// 메모리 관리 함수
extern int mm_init(void);
extern void *mm_malloc(size_t size);
static void *find_fit(size_t aligned_size);
static void place(void *bp, size_t asize);
/***
 * @brief 힙을 확장한다
 * @param size_t 확장할 사이즈 (워드 단위)
 */
static void *extend_heap(size_t words);
extern void mm_free(void *ptr);
extern void *mm_realloc(void *ptr, size_t size);
static void *coalesce(void *bp);

/*
 * Students work in teams of one or two.  Teams enter their team name,
 * personal names and login IDs in a struct of this
 * type in their bits.c file.
 */
typedef struct
{
    char *teamname; /* ID1+ID2 or ID1 */
    char *name1;    /* full name of first member */
    char *id1;      /* login ID of first member */
    char *name2;    /* full name of second member (if any) */
    char *id2;      /* login ID of second member */
} team_t;

extern team_t team;
