#!/bin/bash
cd /workspaces/malloc_lab_docker/malloc-lab
mkdir -p log

run_analysis() {
    echo "=== Valgrind 메모리 분석 시작 ==="
    echo "실행 명령어: valgrind ./mdriver -V -f short1-bal.rep"
    echo ""

    # 더 상세한 Valgrind 옵션
    valgrind \
        --tool=memcheck \
        --track-origins=yes \
        --show-leak-kinds=all \
        --verbose \
        --num-callers=20 \
        --show-below-main=yes \
        --error-limit=no \
        --log-file=log/valgrind_raw.txt \
        ./mdriver -V -f short1-bal.rep

    exit_code=$?
    case $exit_code in
        0)   echo "✅ 프로그램이 정상 종료되었습니다" ;;
        139) echo "💥 Segmentation Fault 발생 (SIGSEGV)" ;;
        134) echo "💥 Abort 신호 발생 (SIGABRT)" ;;
        *)   echo "⚠️  비정상 종료 (exit code: $exit_code)" ;;
    esac

    echo ""
    echo "========================================"
    echo "🚨 크리티컬 에러 (CRITICAL ERRORS) - 반드시 해결 필요!"
    echo "========================================"
    
    # 크래시를 일으키는 심각한 에러들
    critical_errors=""
    
    # Invalid write/read (메모리 경계 위반)
    invalid_access=$(grep -A 8 -B 2 "Invalid write\|Invalid read" log/valgrind_raw.txt)
    if [ -n "$invalid_access" ]; then
        echo "🔴 메모리 경계 위반 (Invalid Memory Access):"
        echo "$invalid_access"
        echo ""
        critical_errors="yes"
    fi
    
    # 프로세스 크래시
    crash_info=$(grep -A 15 -B 5 "Process terminating\|Access not within mapped region\|SIGSEGV\|SIGABRT" log/valgrind_raw.txt)
    if [ -n "$crash_info" ]; then
        echo "💀 프로그램 크래시 정보:"
        echo "$crash_info"
        echo ""
        critical_errors="yes"
    fi
    
    # Double free, Use after free 등
    memory_corruption=$(grep -A 8 -B 2 "double free\|use after free\|free(): invalid pointer" log/valgrind_raw.txt)
    if [ -n "$memory_corruption" ]; then
        echo "⚡ 메모리 손상 에러:"
        echo "$memory_corruption"
        echo ""
        critical_errors="yes"
    fi
    
    if [ -z "$critical_errors" ]; then
        echo "✅ 크리티컬 에러가 발견되지 않았습니다."
    fi

    echo ""
    echo "========================================"
    echo "⚠️  경고 (WARNINGS) - 성능/안정성 개선 권장"
    echo "========================================"
    
    # 초기화되지 않은 값 사용 (경고 수준)
    uninit_warnings=$(grep -A 8 -B 2 "Use of uninitialised value" log/valgrind_raw.txt)
    if [ -n "$uninit_warnings" ]; then
        echo "🟡 초기화되지 않은 값 사용:"
        echo "$uninit_warnings" | head -30
        echo ""
    fi
    
    # 조건부 점프 (경고 수준)
    conditional_warnings=$(grep -A 5 -B 2 "Conditional jump\|depends on uninitialised" log/valgrind_raw.txt)
    if [ -n "$conditional_warnings" ]; then
        echo "🟡 초기화되지 않은 값 기반 조건 분기:"
        echo "$conditional_warnings" | head -20
        echo ""
    fi
    
    # 가능한 메모리 누수 (경고 수준)
    possible_leaks=$(grep -A 5 "possibly lost:" log/valgrind_raw.txt)
    if [ -n "$possible_leaks" ]; then
        echo "🟡 가능한 메모리 누수:"
        echo "$possible_leaks"
        echo ""
    fi
    
    # 일반 경고들
    general_warnings=$(grep -i "warning" log/valgrind_raw.txt)
    if [ -n "$general_warnings" ]; then
        echo "🟡 일반 경고:"
        echo "$general_warnings"
        echo ""
    fi

    echo ""
    echo "========================================"
    echo "💧 메모리 누수 분석"
    echo "========================================"
    
    # 확실한 메모리 누수 (해결 권장)
    definite_leaks=$(grep -A 2 "definitely lost:" log/valgrind_raw.txt)
    if [ -n "$definite_leaks" ]; then
        echo "🔴 확실한 메모리 누수 (해결 필요):"
        echo "$definite_leaks"
        echo ""
    fi
    
    # 전체 메모리 요약
    heap_summary=$(grep -A 15 "HEAP SUMMARY\|LEAK SUMMARY" log/valgrind_raw.txt)
    if [ -n "$heap_summary" ]; then
        echo "📊 메모리 사용 요약:"
        echo "$heap_summary"
    fi

    echo ""
    echo "========================================"
    echo "📊 심각도별 통계"
    echo "========================================"
    
    total_lines=$(wc -l < log/valgrind_raw.txt)
    
    # 크리티컬 에러 카운트
    invalid_count=$(grep -c "Invalid write\|Invalid read" log/valgrind_raw.txt)
    crash_count=$(grep -c "Process terminating\|SIGSEGV\|SIGABRT" log/valgrind_raw.txt)
    corruption_count=$(grep -c "double free\|use after free" log/valgrind_raw.txt)
    
    # 경고 카운트
    uninit_count=$(grep -c "Use of uninitialised value" log/valgrind_raw.txt)
    conditional_count=$(grep -c "Conditional jump.*uninitialised" log/valgrind_raw.txt)
    warning_count=$(grep -c -i "warning" log/valgrind_raw.txt)
    
    # 메모리 누수 카운트
    definite_leak_count=$(grep -c "definitely lost:" log/valgrind_raw.txt)
    possible_leak_count=$(grep -c "possibly lost:" log/valgrind_raw.txt)

    echo "📁 원본 로그: log/valgrind_raw.txt ($total_lines 줄)"
    echo "📁 분석 결과: log/valgrind_output.txt"
    echo ""
    echo "🚨 크리티컬 에러 (반드시 수정):"
    echo "   🔴 메모리 경계 위반: $invalid_count"
    echo "   💀 프로그램 크래시: $crash_count" 
    echo "   ⚡ 메모리 손상: $corruption_count"
    echo ""
    echo "⚠️  경고 (개선 권장):"
    echo "   🟡 초기화 안된 값: $uninit_count"
    echo "   🟡 조건부 분기 문제: $conditional_count"
    echo "   🟡 일반 경고: $warning_count"
    echo ""
    echo "💧 메모리 누수:"
    echo "   🔴 확실한 누수: $definite_leak_count"
    echo "   🟡 가능한 누수: $possible_leak_count"

    total_critical=$((invalid_count + crash_count + corruption_count))
    total_warnings=$((uninit_count + conditional_count + warning_count))

    echo ""
    if [ $total_critical -gt 0 ]; then
        echo "🚨 우선순위 1: 크리티컬 에러 $total_critical개를 먼저 해결하세요!"
        echo "🔍 추천 디버깅 단계:"
        echo "1. gdb ./mdriver  # GDB로 크래시 위치 정확히 찾기"
        echo "2. (gdb) run -V -f short1-bal.rep"
        echo "3. (gdb) bt  # 스택 트레이스 확인"
    elif [ $total_warnings -gt 0 ]; then
        echo "⚠️  우선순위 2: 경고 $total_warnings개를 개선하면 안정성이 향상됩니다."
        echo "🔍 권장 조치:"
        echo "1. 변수 초기화 확인"
        echo "2. 메모리 할당 후 초기화"
        echo "3. 경계 검사 추가"
    else
        echo "✅ 모든 검사를 통과했습니다!"
    fi

    echo ""
    echo "=== 분석 완료 ==="
}

run_analysis > log/valgrind_output.txt 2>&1

echo "✅ Valgrind 분석이 완료되었습니다!"
echo "📁 결과 파일: log/valgrind_output.txt"
echo "📁 원본 로그: log/valgrind_raw.txt"

# 간단한 요약도 터미널에 출력
critical_count=$(grep -c "Invalid write\|Invalid read\|Process terminating" log/valgrind_raw.txt)
warning_count=$(grep -c "Use of uninitialised value" log/valgrind_raw.txt)

echo ""
if [ $critical_count -gt 0 ]; then
    echo "🚨 크리티컬 에러 $critical_count개 발견! 즉시 수정 필요"
elif [ $warning_count -gt 0 ]; then
    echo "⚠️  경고 $warning_count개 발견. 개선 권장"
else
    echo "✅ 에러 없음"
fi
echo ""
echo "자세한 결과: cat log/valgrind_output.txt"