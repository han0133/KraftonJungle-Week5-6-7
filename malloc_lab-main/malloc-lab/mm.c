#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"
#include "src/mm_utils.h"
#include "src/mm_block.h"
#define increase_heap mem_sbrk

team_t team = {
    "7",
    "안소영",
    "soyoungahn2706@gmail.com",
    "",
    ""};

#define WORD_SIZE 8
#define DOUBLE_SIZE 16
#define CHUNKSIZE 4096                           /* 4kb(4096바이트)씩 힙을 확장할 것임 */
#define PACK(size, alloc) ((size) | (alloc))     /* 블록의 크기와 할당 상태를 하나의 워드로 결합 */
#define SIZE_T_SIZE (align_size(sizeof(size_t))) /* 최소 할당 단위 정의 */

static char *prologue_payload = 0;

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* 힙 초기화: 32kb */
    char *heap_base_ptr = NULL;
    if ((heap_base_ptr = increase_heap(4 * WORD_SIZE)) == (void *)-1)
        return -1;

    update_value(heap_base_ptr, PACK(0, 0));                             /* 정렬 패딩 : 0 */
    update_value(heap_base_ptr + (1 * WORD_SIZE), PACK(DOUBLE_SIZE, 1)); /* 프롤로그헤더 설정 */
    update_value(heap_base_ptr + (2 * WORD_SIZE), PACK(DOUBLE_SIZE, 1)); /* 프롤로그푸터 설정 */
    update_value(heap_base_ptr + (3 * WORD_SIZE), PACK(0, 1));           /* 에필로그헤더 설정 */

    int payload_start_index = WORD_SIZE * 2;

    prologue_payload = heap_base_ptr + payload_start_index;

    /* 초기 가용블록 512kb 생성*/
    if (extend_heap(CHUNKSIZE / WORD_SIZE) == NULL)
        return -1;

    return 0;
}

/*
 * mm_malloc - 사용자가 요청한 사이즈만큼 동적으로 메모리 할당
 */
void *mm_malloc(size_t size)
{
    int aligned_size = align_size(size + SIZE_T_SIZE);
    char *p_new_block;

    // case1. 가용영역에서 할당할 위치를 찾고 할당
    if ((p_new_block = (find_fit(aligned_size))) != NULL)
    {
        place(p_new_block, aligned_size);
        return p_new_block;
    }

    // case2. 할당 가능 영역이 없으면 heap을 확장
    int max = aligned_size > CHUNKSIZE ? aligned_size : CHUNKSIZE;
    p_new_block = extend_heap(max / WORD_SIZE);
    if (p_new_block == NULL)
        return NULL;
    else
    {
        place(p_new_block, aligned_size);
        return p_new_block;
    }
}

/**
 * @brief first fit방식으로 가용 영역 탐색
 * @param aligned_size 정렬된 블록 사이즈
 */
static void *find_fit(size_t aligned_size)
{
    void *p_block;
    for (p_block = prologue_payload; get_size(get_header_pointer(p_block)) > 0; p_block = get_next_payload_pointer(p_block))
    {
        if (!is_allocated(get_header_pointer(p_block)) && aligned_size <= get_size(get_header_pointer(p_block)))
        {
            return p_block;
        }
    }
    return NULL;
}

/**
 * @brief 블록을 할당하고, 남는 공간이 충분하면 분할하여 새로운 free 블록 생성
 * @param p_payload 블록의 페이로드 포인터
 * @param asize 실제 할당할 크기(헤더+페이로드+푸터)
 */
static void place(void *p_payload, size_t asize)
{
    size_t current_block_size = get_size(get_header_pointer(p_payload));

    // 가용영역의 크기 - 필요한 영역 = 24byte 이상이면 분할
    if ((current_block_size - asize) >= (3 * WORD_SIZE)) /* 헤더(8) + 최소 페이로드(8) + 푸터(8) */
    {
        update_value(get_header_pointer(p_payload), PACK(asize, 1));
        update_value(get_footer_pointer(p_payload), PACK(asize, 1));
        p_payload = get_next_payload_pointer(p_payload);
        update_value(get_header_pointer(p_payload), PACK(current_block_size - asize, 0));
        update_value(get_footer_pointer(p_payload), PACK(current_block_size - asize, 0));
    }
    else
    {
        update_value(get_header_pointer(p_payload), PACK(current_block_size, 1));
        update_value(get_footer_pointer(p_payload), PACK(current_block_size, 1));
    }
}

/**
 * @brief 헤더와 푸터 가용상태 표시 초기화 및 병합함수 호출
 * @param payload_ptr 현재 블록의 페이로드 포인터
 */
void mm_free(void *payload_ptr)
{
    size_t size = get_size(get_header_pointer(payload_ptr)); // 블록 사이즈

    update_value(get_header_pointer(payload_ptr), PACK(size, 0));
    update_value(get_footer_pointer(payload_ptr), PACK(size, 0));
    coalesce(payload_ptr);
}

/**
 * @brief 이미 할당한 블록의 크기를 변경
 * @param payload_ptr 현재 블록의 페이로드 포인터
 * @param size 변경하고자 하는 크기
 */
void *mm_realloc(void *payload_ptr, size_t newsize)
{
    if (newsize <= 0) // realloc에서 크기 0 요청은 기존 블록을 free시키라는 의미임
    {
        mm_free(payload_ptr);
        return 0;
    }
    if (payload_ptr == NULL)
    {
        return mm_malloc(newsize); // realloc에서 기존 포인터가 NULL이라는 것은 새 메모리 할당 요청임
    }

    size_t oldsize = get_size(get_header_pointer(payload_ptr)) - DOUBLE_SIZE;
    if (newsize <= oldsize)
    {
        if (oldsize - newsize >= 3 * WORD_SIZE)
        {
            size_t newBlockSize = oldsize - newsize;

            // 1. 현재 블록 앞부분 헤더/푸터 수정
            update_value(get_header_pointer(payload_ptr), PACK(newsize, 1));
            update_value(get_footer_pointer(payload_ptr), PACK(newsize, 1));

            // 2. 남는 영역은 free
            void *payload_ptrFree = (char *)payload_ptr + newsize;
            update_value(get_header_pointer(payload_ptrFree), PACK(newBlockSize, 0));
            update_value(get_footer_pointer(payload_ptrFree), PACK(newBlockSize, 0));

            coalesce(payload_ptrFree);
        }
        return payload_ptr;
    }
    else
    {
        void *newpayload_ptr = mm_malloc(newsize);
        if (newpayload_ptr == NULL)
            return 0;

        memcpy(newpayload_ptr, payload_ptr, oldsize);
        mm_free(payload_ptr);
        return newpayload_ptr;
    }
}

static void *extend_heap(size_t words)
{
    char *payload_ptr;
    size_t size;

    /* 정렬 */
    size = (words % 2) ? (words + 1) * WORD_SIZE : words * WORD_SIZE;
    if ((long)(payload_ptr = increase_heap(size)) == -1) // payload_ptr: 확장된 메모리의 시작주소
        return NULL;

    /* 새로 생긴 블록의 헤더/푸터를 초기화하고, 에필로그 헤더를 수정한다 */
    update_value(get_header_pointer(payload_ptr), PACK(size, 0));
    update_value(get_footer_pointer(payload_ptr), PACK(size, 0));
    update_value(get_header_pointer(get_next_payload_pointer(payload_ptr)), PACK(0, 1));

    return coalesce(payload_ptr);
}

/** TODO: 에필로그 헤더 수정 부분 있는지 체크 */
static void *coalesce(void *payload_ptr)
{
    // get_size(get_header_pointer(get_prev_payload_pointer(payload_ptr)));

    bool prev_alloc = is_allocated(get_footer_pointer(get_prev_payload_pointer(payload_ptr)));
    bool next_alloc = is_allocated(get_header_pointer(get_next_payload_pointer(payload_ptr)));
    size_t size = get_size(get_header_pointer(payload_ptr));

    if (prev_alloc && next_alloc) /* case1. 양쪽 블록이 모두 할당된 경우 */
    {
        return payload_ptr;
    }
    else if (prev_alloc && !next_alloc) /* case2. 이전 블록 할당됨, 다음 블록 가용 */
    {
        size += get_size(get_header_pointer(get_next_payload_pointer(payload_ptr)));
        update_value(get_header_pointer(payload_ptr), PACK(size, 0));
        update_value(get_footer_pointer(payload_ptr), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc) /* case3. 이전 블록 가용, 다음 블록 할당됨 */
    {
        void *prev_payload_pointer = get_prev_payload_pointer(payload_ptr);
        size += get_size(get_header_pointer(prev_payload_pointer));
        update_value(get_header_pointer(prev_payload_pointer), PACK(size, 0));
        update_value(get_footer_pointer(payload_ptr), PACK(size, 0));
        payload_ptr = prev_payload_pointer;
    }
    else /* case4. 이전, 다음 블록 가용 */
    {
        size += get_size(get_header_pointer(get_prev_payload_pointer(payload_ptr))) + get_size(get_footer_pointer(get_next_payload_pointer(payload_ptr)));
        update_value(get_header_pointer(get_prev_payload_pointer(payload_ptr)), PACK(size, 0));
        update_value(get_footer_pointer(get_next_payload_pointer(payload_ptr)), PACK(size, 0));
        payload_ptr = get_prev_payload_pointer(payload_ptr);
    }
    return payload_ptr;
}
