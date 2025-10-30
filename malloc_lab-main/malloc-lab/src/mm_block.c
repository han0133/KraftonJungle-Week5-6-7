#include "mm_utils.h"
#include "mm_block.h"

#ifndef WORD_SIZE
#define WORD_SIZE 8
#define DOUBLE_SIZE 16
#endif

/**
 * @brief 블록의 크기를 반환하는 함수
 * @param header_ptr 헤더 포인터
 */
size_t get_size(void *header_ptr)
{
  // void *result = get_value(header_ptr) & ~0XF;
  return get_value(header_ptr) & ~0XF;
}

/**
 * @brief 할당 비트를 반환하는 함수 (1: 할당됨, 0: 사용가능)
 * @param header_ptr 헤더 포인터
 * @return 할당 비트 (0 또는 1)
 */
bool is_allocated(void *header_ptr)
{
  return get_value(header_ptr) & 0x1;
}

/**
 * @brief 블록의 헤더 위치를 반환하는 함수
 * @param payload_ptr 페이로드 시작 주소
 * @return 헤더 시작 주소
 */
void *get_header_pointer(void *payload_ptr)
{
  return (char *)payload_ptr - WORD_SIZE;
}

/**
 * @brief 블록의 푸터 위치를 반환하는 함수
 * @param payload_ptr 페이로드 시작 주소
 * @note 헤더(WORD) + payload + 푸터(WORD)
 * @return 푸터 시작 주소
 */
void *get_footer_pointer(void *payload_ptr)
{
  return (char *)payload_ptr + get_size(get_header_pointer(payload_ptr)) - (WORD_SIZE * 2);
}

/**
 * @brief 다음 블록의 페이로드 위치를 반환하는 함수
 * @param payload_ptr 현재 페이로드 시작 주소
 * @return 다음 블록의 페이로드 시작 주소
 */
void *get_next_payload_pointer(void *payload_ptr)
{
  return (char *)payload_ptr + get_size((char *)payload_ptr - WORD_SIZE);
}

/**
 * @brief 이전 블록의 페이로드 위치를 반환하는 함수
 * @param payload_ptr 현재 페이로드 시작 주소
 * @return 이전 블록의 페이로드 시작 주소
 */
void *get_prev_payload_pointer(void *payload_ptr)
{
  return (char *)payload_ptr - get_size((char *)payload_ptr - DOUBLE_SIZE);
}