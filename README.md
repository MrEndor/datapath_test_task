# External Sort

External K-way merge sort over tape abstraction.

Reads a binary tape file of `int32_t` values, sorts it using a configurable RAM limit,
and writes the result to an output tape file.

## Prerequisites

- `cmake >= 3.20`
- `g++` or `clang++` with C++23 support
- `just` (optional)

## Quick start

1. Build

```bash
cmake --preset release
cmake --build --preset release --parallel
```

or

```bash
just build
```

2. Run

```bash
./build/src/external_sorting input.bin output.bin
```

or

```bash
just run "input.bin output.bin"
```

## Options

| Flag | Default | Description |
|------|---------|-------------|
| `--config, -c` | `./config.yaml` | YAML config file |
| `--memory, -m` | from config | RAM limit — `64M`, `256K`, `1G`, or raw bytes |
| `input` | required | Input tape file (binary `int32_t` array) |
| `output` | required | Output tape file (must already exist with correct size) |

## Config file

```yaml
tape:
  read_delay_ms: 0    # delay per Read()
  write_delay_ms: 0   # delay per Write()
  rewind_delay_ms: 0  # delay per Rewind()
  shift_delay_ms: 0   # delay per MoveForward()/MoveBackward()

sorter:
  ram_limit_bytes: 67108864  # 64 MiB
  tmp_dir: ./tmp
```

## Testing

```bash
just test
```

## Development

```bash
just fmt       # format code
just lint      # clang-tidy
just coverage  # coverage report
just check     # fmt-check + lint + test
```
