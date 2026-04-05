# AGENTS.md

## Project Goal

This is a high-performance, non-JIT Brainfuck interpreter written in C++23. The primary objective is to be the fastest interpreter-style (non-JIT) Brainfuck implementation. The benchmark target is `samples/mandelbrot.b` execution time (currently 0.77s, vs next-best competitor at 0.98s). Competitor benchmarks are listed in `readme.md`.

## Architecture

Three-stage pipeline orchestrated by `src/brainfuck/go.cpp`:

1. **File reading** (`file.cpp`) — reads a `.b` source file into `std::vector<char>`
2. **Parsing + optimization** (`parse.cpp`) — transforms source characters into an optimized `std::vector<Instruction>`
3. **Execution** (`execute.cpp`) — runs the instruction vector via GCC computed gotos

```
.b file → read() → vector<char> → parse() → vector<Instruction> → execute()
```

## Instruction Representation (`instruction.h`)

The `Instruction` struct uses a union-based layout with three 64-bit fields (192 bits total):

| Field | Union with | Purpose |
|-------|-----------|---------|
| `type` (Type enum, uint64_t) | `jump` (void*) | Instruction kind at parse time; replaced with computed goto label address before execution |
| `value` (int64_t) | `next` (Instruction*) | Operand value at parse time; replaced with instruction pointer for jumps/transfers before execution |
| `move` (int64_t) | — | Data pointer offset applied before this instruction executes |

The 192-bit struct size is deliberate — changing it has a measured 13% performance impact due to cache alignment and code locality effects.

Instruction types:
- `NOOP` — no operation
- `DONE` — program termination
- `DATA_ADD` — add value to current cell (`+`/`-`)
- `DATA_SET_FROM_INPUT` — read from stdin (`,`)
- `DATA_PRINT` — write to stdout (`.`)
- `DATA_POINTER_ADD` — move data pointer (`>`/`<`), squashed into `move` field during parsing
- `INSTRUCTION_POINTER_SET_IF_ZERO` — jump forward if zero (`[`)
- `INSTRUCTION_POINTER_SET_IF_NOT_ZERO` — jump back if non-zero (`]`), also handles scan loops
- `DATA_TRANSFER` — optimized loop pattern (`[-]`, `[->+<]`, `[->++>+++<<]`)
- `DATA_TRANSFER_META` — transfer destination/multiplier metadata following a `DATA_TRANSFER`

## Parser Optimizations (`parse.cpp`)

Optimization passes applied in order:

1. **Strip non-BF characters** (`removeNoopCodes`) — removes anything that isn't `+-><.,[]`
2. **Fold consecutive ops** (`consecutiveAccumulator`) — e.g. `+++` becomes `DATA_ADD, value=3`; adjacent same-type instructions are merged via `addValue`
3. **Loop-to-transfer conversion** (`tryOptimiseLoop`) — detects patterns like `[-]`, `[->+<]`, `[->++>+++<<]` and converts them to a single `DATA_TRANSFER` instruction followed by `DATA_TRANSFER_META` entries. Requirements: loop must return to origin cell (`offset == 0`) and decrement origin by exactly 1 (`transfers[0] == -1`)
4. **Pointer movement squashing** (`squashPointerAddInstructions`) — folds `DATA_POINTER_ADD` instructions into the `move` field of the following instruction, eliminating them as separate instructions
5. **Trailing jump merge** (`optimize`) — merges a closing bracket into the preceding instruction when they share the same value and move
6. **Bracket resolution** (`applyInstructionPointerOffsets`) — converts bracket pair IDs into instruction pointer offsets for `[` and `]`

## Execution Engine (`execute.cpp`)

Performance-critical details:

- **GCC computed gotos** (`goto*`) are used for instruction dispatch instead of a switch statement. This is a GCC/Clang extension and will not compile on MSVC.
- `setupInstructionAddresses` runs once before execution, replacing each instruction's `type` enum with the corresponding label address from `jumpTable[]`, and converting `value` fields to `Instruction*` pointers for jumps and transfers.
- **Data tape**: `int8_t datas[30000]` — standard BF spec, stack-allocated.
- **Scan loop optimization**: `INSTRUCTION_POINTER_SET_IF_NOT_ZERO` detects self-referencing loops (`instruction->next == instruction`) and executes them as a tight `while` loop scanning the tape, avoiding dispatch overhead.
- **Data masking**: `*data & 255` is used for zero-checks to correctly handle int8_t wrapping behavior.
- **I/O**: `output()` and `input()` handle repeated print/read operations via loop over `times` parameter. Input uses `std::noskipws` and checks for EOF.

## Build System

- **CMake** 3.28.3+ with **Ninja** generator
- **Compiler**: g++ with C++23 standard
- **Optimization flags**: `-march=native -Ofast -Wall -Wextra -Werror -Wsign-conversion -Wconversion`
- **Library split**: `brainfuck-execute` (shared lib, just `execute.cpp`) and `brainfuck` (shared lib, everything else). This split and the source file ordering in `src/brainfuck/CMakeLists.txt` affects code locality — reordering has a measured 4% performance impact.

Build and run:
```sh
cmake --preset release
cmake --build --preset all
./build/src/standalone/brainfuck samples/mandelbrot.b
./build/src/tests/tests
```

Build presets: `all` (everything), `standalone` (just the CLI binary).

## Testing

- **Framework**: doctest v2.4.12 (fetched via CMake FetchContent)
- **Snapshot tests** in `src/tests/snapshots/` with two categories:
  - `parse/` — formatted instruction output from the parser
  - `go/` — stdout captured from end-to-end execution
- **Test files**: `src/tests/parse.cpp` (parser snapshots), `src/tests/go.cpp` (execution snapshots), `src/tests/file.cpp` (file reading)
- **Test utilities**: `src/tests/utils.cpp` provides `REQUIRE_SNAPSHOT` (snapshot comparison) and `startCapturingIO` (stdout/stdin redirection)
- **Sample programs** in `samples/` — includes unit-test `.b` files in `samples/tests/` and larger programs (`mandelbrot.b`, `life.b`, `hello_world.b`, etc.)

## Performance Optimization Guidelines

These constraints are backed by documented benchmark results in `readme.md`:

- **Do not change the Instruction struct size** — the 192-bit (3x 64-bit) layout is deliberate; changing it caused a 13% performance swing
- **Do not reorder source files** in `src/brainfuck/CMakeLists.txt` without benchmarking — file order affects code locality (4% impact)
- **Do not remove seemingly dead code** without benchmarking — removing dead code has caused 8% performance loss due to code layout changes
- **Do not change constant values** without benchmarking — a constant value change once caused 15% performance loss
- **Do not use smaller integer types** for instruction fields — 64-bit types are 11% faster than 8-bit on modern CPUs
- **Always benchmark** changes with `samples/mandelbrot.b` using `hyperfine`
- **Computed gotos are GCC/Clang-specific** — this project does not target MSVC portability
