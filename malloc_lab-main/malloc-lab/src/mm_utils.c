#include "mm_utils.h"
#include <stdbool.h>

#ifndef WORD_SIZE
#define WORD_SIZE 8
#endif

#ifndef DOUBLE_SIZE
#define DOUBLE_SIZE 16
#endif

/**
 * @brief 크기를 16배수로 올림 정렬하는 함수
 * @param size 정렬할 크기 (바이트단위)
 * @return 16바이트로 정렬된 크기
 */
size_t align_size(size_t size)
{
  return (size + (DOUBLE_SIZE - 1)) & ~0xF;
}

/**
 * @brief 포인터 변수 ptr에 값을 넣는 함수
 * @param ptr 포인터 변수
 * @param val 대입할 값
 */
void update_value(void *ptr, unsigned long val)
{
  *(unsigned long *)ptr = val;
}

/**
 * @brief 포인터 변수 ptr의 값을 가져오는 함수
 */
unsigned long get_value(void *ptr)
{
  // unsigned long *result = *(unsigned long *)ptr;
  return *(unsigned long *)ptr;
}

/**
 * @brief 모든 블록 정보 표시
 * @param prologue_payload
 * @param payload_ptr
 */
void debug(void *prologue_payload, void *payload_ptr)
{
  printf("=== CURRENT BLOCK INFO ===\n");
  printf("CURRENT BLOCK Payload: %p\n", payload_ptr);
  void *header_ptr = get_header_pointer(payload_ptr);
  void *footer_ptr = get_footer_pointer(payload_ptr);

  size_t size = get_size(header_ptr);
  bool allocated = is_allocated(header_ptr);

  printf("  Header Address:  %p -> Value: 0x%lX (Size: %zu, Alloc: %d)\n",
         header_ptr, get_value(header_ptr), size, allocated);
  printf("  Footer Address:  %p -> Value: 0x%lX (Size: %zu, Alloc: %d)\n",
         footer_ptr, get_value(footer_ptr), get_size(footer_ptr), is_allocated(footer_ptr));
  printf("  Block Size: %zu bytes\n\n", size);

  printf("=== HEAP BLOCKS INFO ===\n");
  printf("Heap Start: %p\n", prologue_payload - WORD_SIZE);
  printf("Prologue Payload: %p\n\n", prologue_payload);

  int block_count = 0;
  void *current_payload = prologue_payload;

  while (get_size(get_header_pointer(current_payload)) > 0)
  {
    void *header_ptr = get_header_pointer(current_payload);
    void *footer_ptr = get_footer_pointer(current_payload);

    size_t size = get_size(header_ptr);
    bool allocated = is_allocated(header_ptr);

    printf("Block %d:\n", block_count++);
    printf("  Payload Address: %p\n", current_payload);
    printf("  Header Address:  %p -> Value: 0x%lX (Size: %zu, Alloc: %d)\n",
           header_ptr, get_value(header_ptr), size, allocated);
    printf("  Footer Address:  %p -> Value: 0x%lX (Size: %zu, Alloc: %d)\n",
           footer_ptr, get_value(footer_ptr), get_size(footer_ptr), is_allocated(footer_ptr));
    printf("  Block Size: %zu bytes\n", size);
    printf("  Status: %s\n\n", allocated ? "ALLOCATED" : "FREE");

    current_payload = get_next_payload_pointer(current_payload);
  }

  // 에필로그 블록 정보
  void *epilogue_header = get_header_pointer(current_payload);
  printf("Epilogue Header: %p -> Value: 0x%lX\n",
         epilogue_header, get_value(epilogue_header));
  printf("========================\n\n");
}