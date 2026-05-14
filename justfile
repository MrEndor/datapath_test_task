set shell := ["bash", "-eou", "pipefail", "-c"]

default:
    @just --list

build:
    cmake --preset release
    cmake --build --preset release --parallel

test:
    cmake --preset debug-asan
    cmake --build --preset debug-asan --parallel
    ctest --preset debug-asan

coverage:
    cmake --preset coverage
    cmake --build --preset coverage --parallel
    ctest --preset coverage
    cmake --build --preset coverage --target coverage_report

fmt:
    find include src tests -name '*.hpp' -o -name '*.cpp' -o -name '*.ipp' 2>/dev/null \
        | xargs --no-run-if-empty clang-format -i

fmt-check:
    find include src tests -name '*.hpp' -o -name '*.cpp' -o -name '*.ipp' 2>/dev/null \
        | xargs --no-run-if-empty clang-format --dry-run --Werror

lint:
    cmake --preset release
    cmake --build --preset release --parallel
    find include src \( -name '*.cpp' -o -name '*.hpp' \) -not -name '*.ipp' 2>/dev/null \
        | xargs --no-run-if-empty clang-tidy -p build

run input:
    ./build/src/external_sorting {{input}}

clean:
    rm -rf build build-test build-test-cov

check: fmt-check lint test
