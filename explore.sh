#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
EXPLORATIONS_DIR="$REPO_ROOT/explorations"
TIMESTAMP=$(date +%Y%m%d-%H%M%S)
BENCHMARK_CMD="./build/src/standalone/brainfuck samples/mandelbrot.b"
HYPERFINE_ARGS="--warmup 3 --runs 10"
BASELINE_JSON="/tmp/bf-explore-baseline-${TIMESTAMP}.json"
RESULT_JSON="/tmp/bf-explore-result-${TIMESTAMP}.json"

# AI backend: "cursor" (default) or "claude"
AI_BACKEND="${BF_EXPLORE_AI:-cursor}"

# --- helpers ---------------------------------------------------------------

log()  { printf "\n\033[1;34m==> %s\033[0m\n" "$*"; }
err()  { printf "\n\033[1;31m==> ERROR: %s\033[0m\n" "$*" >&2; }
bail() { err "$@"; exit 1; }

check_tool() {
  command -v "$1" &>/dev/null || bail "'$1' is not installed. $2"
}

ai_readonly() {
  local prompt="$1"
  case "$AI_BACKEND" in
    cursor) cursor agent -p "$prompt" ;;
    claude) claude -p --allowedTools "Read,Glob,Grep" "$prompt" ;;
    *)      bail "Unknown AI backend: $AI_BACKEND. Set BF_EXPLORE_AI to 'cursor' or 'claude'." ;;
  esac
}

ai_write() {
  local prompt="$1"
  case "$AI_BACKEND" in
    cursor) cursor agent -p --force "$prompt" ;;
    claude) claude -p --allowedTools "Read,Write,Edit,Glob,Grep,Bash" "$prompt" ;;
    *)      bail "Unknown AI backend: $AI_BACKEND" ;;
  esac
}

cleanup_on_failure() {
  local exit_code=$?
  if [ $exit_code -ne 0 ]; then
    err "Script failed (exit $exit_code). Returning to main branch."
    git checkout main 2>/dev/null || true
  fi
}
trap cleanup_on_failure EXIT

# --- preflight checks ------------------------------------------------------

log "Checking prerequisites (AI backend: $AI_BACKEND)"

case "$AI_BACKEND" in
  cursor) check_tool cursor "Install Cursor CLI: curl https://cursor.com/install -fsSL | bash" ;;
  claude) check_tool claude "Install Claude Code: npm install -g @anthropic-ai/claude-code" ;;
  *)      bail "Unknown AI backend: $AI_BACKEND. Set BF_EXPLORE_AI to 'cursor' or 'claude'." ;;
esac

check_tool hyperfine "Install: cargo install hyperfine  OR  apt install hyperfine"
check_tool jq        "Install: apt install jq"
check_tool cmake     "Install cmake >= 3.28.3"
check_tool ninja     "Install ninja-build"
check_tool g++       "Install g++ with C++23 support"
check_tool bc        "Install: apt install bc"

if [ -n "$(git status --porcelain)" ]; then
  bail "Working tree is dirty. Commit or stash changes before running."
fi

CURRENT_BRANCH=$(git branch --show-current)
if [ "$CURRENT_BRANCH" != "main" ]; then
  bail "Must be on the 'main' branch to start. Currently on '$CURRENT_BRANCH'."
fi

mkdir -p "$EXPLORATIONS_DIR"

# --- step 1: baseline benchmark -------------------------------------------

log "Building current main for baseline benchmark"
cmake --preset release
cmake --build --preset all

log "Running baseline benchmark"
hyperfine $HYPERFINE_ARGS --export-json "$BASELINE_JSON" "$BENCHMARK_CMD"
BASELINE_MEAN=$(jq '.results[0].mean' "$BASELINE_JSON")
BASELINE_STDDEV=$(jq '.results[0].stddev' "$BASELINE_JSON")
log "Baseline: ${BASELINE_MEAN}s (+/- ${BASELINE_STDDEV}s)"

# --- step 2: AI generates optimisation plan --------------------------------

log "Asking AI to generate an optimisation plan"

EXISTING_EXPLORATIONS=""
for f in "$EXPLORATIONS_DIR"/*.md; do
  [ -f "$f" ] || continue
  EXISTING_EXPLORATIONS+="
--- $(basename "$f") ---
$(cat "$f")
"
done

read -r -d '' PLAN_PROMPT << 'PLAN_PROMPT_END' || true
You are analysing a high-performance Brainfuck interpreter written in C++23.

Read the AGENTS.md file and all source files under src/brainfuck/ to understand the codebase.
PLAN_PROMPT_END

PLAN_PROMPT+="

The following optimisation explorations have ALREADY been attempted. Do NOT propose any of these again, even if they failed:

${EXISTING_EXPLORATIONS:-"(none yet)"}
"

read -r -d '' PLAN_PROMPT_TAIL << 'PLAN_PROMPT_TAIL_END' || true
Propose ONE specific, concrete performance optimisation that has NOT been tried before.
Focus on ideas that could measurably improve execution time of samples/mandelbrot.b.
Consider: compiler hints, memory layout, loop optimisations, instruction dispatch, cache effects, SIMD, PGO, LTO, or algorithmic improvements to the parser/executor.

Output ONLY a markdown document in this exact format (no other text before or after):

# <Short Descriptive Title>

## Status
pending

## Hypothesis
<What you expect to improve and why, with technical reasoning>

## Approach
<Specific code changes needed, referencing exact files and functions>

## Risk Assessment
<What could go wrong, performance sensitivity concerns>
PLAN_PROMPT_TAIL_END

PLAN_PROMPT+="$PLAN_PROMPT_TAIL"

cd "$REPO_ROOT"
PLAN_OUTPUT=$(ai_readonly "$PLAN_PROMPT") || bail "AI failed to generate plan"

TITLE=$(echo "$PLAN_OUTPUT" | grep -m1 '^# ' | sed 's/^# *//')
SLUG=$(echo "$TITLE" | tr '[:upper:]' '[:lower:]' | sed 's/[^a-z0-9]/-/g' | sed 's/--*/-/g' | sed 's/^-//;s/-$//')

if [ -z "$SLUG" ]; then
  echo "--- AI output was ---"
  echo "$PLAN_OUTPUT"
  echo "---"
  bail "AI produced empty or unparseable plan title"
fi

BRANCH_NAME="explorations-${SLUG}"
EXPLORATION_FILE="$EXPLORATIONS_DIR/${TIMESTAMP}-${SLUG}.md"

echo "$PLAN_OUTPUT" > "$EXPLORATION_FILE"
log "Plan saved: $EXPLORATION_FILE"
log "Title: $TITLE"

# --- step 3: create branch ------------------------------------------------

log "Creating branch: $BRANCH_NAME"
git checkout -b "$BRANCH_NAME"

# --- step 4: AI implements the plan ---------------------------------------

log "Asking AI to implement the optimisation"

IMPLEMENT_PROMPT="You are implementing a performance optimisation for a Brainfuck interpreter.

Read the plan file at: $EXPLORATION_FILE
Read AGENTS.md for critical constraints (struct sizes, file ordering, etc).

Implement ONLY what the plan describes. Make the minimum necessary code changes.
Do NOT change the Instruction struct size (192 bits / 3x 64-bit fields).
Do NOT reorder files in src/brainfuck/CMakeLists.txt.

After making changes, build and verify:
  cmake --preset release
  cmake --build --preset all
  ./build/src/tests/tests

If tests fail, try to fix the issue. If you cannot fix it, leave the code as-is."

cd "$REPO_ROOT"
ai_write "$IMPLEMENT_PROMPT" || {
  err "AI implementation phase encountered an error (continuing anyway)"
}

# --- step 5: build and test -----------------------------------------------

log "Building and testing the changes"
cd "$REPO_ROOT"
BUILD_OK=true
TEST_OK=true

cmake --preset release && cmake --build --preset all || {
  err "Build failed"
  BUILD_OK=false
}

if $BUILD_OK; then
  ./build/src/tests/tests || {
    err "Tests failed"
    TEST_OK=false
  }
fi

# --- step 6: benchmark (only if build + tests passed) ---------------------

RESULT_MEAN="N/A"
RESULT_STDDEV="N/A"
CHANGE_PCT="N/A"
STATUS="failure"

if $BUILD_OK && $TEST_OK; then
  log "Running benchmark on optimised build"
  hyperfine $HYPERFINE_ARGS --export-json "$RESULT_JSON" "$BENCHMARK_CMD"
  RESULT_MEAN=$(jq '.results[0].mean' "$RESULT_JSON")
  RESULT_STDDEV=$(jq '.results[0].stddev' "$RESULT_JSON")
  CHANGE_PCT=$(echo "scale=2; (($RESULT_MEAN - $BASELINE_MEAN) / $BASELINE_MEAN) * 100" | bc)

  if (( $(echo "$CHANGE_PCT < -0.5" | bc -l) )); then
    STATUS="success"
  elif (( $(echo "$CHANGE_PCT > 0.5" | bc -l) )); then
    STATUS="regression"
  else
    STATUS="no-improvement"
  fi

  log "Result: ${RESULT_MEAN}s (+/- ${RESULT_STDDEV}s) | change: ${CHANGE_PCT}%"
else
  log "Skipping benchmark due to build/test failure"
fi

# --- step 7: commit and push the branch -----------------------------------

log "Committing changes on branch $BRANCH_NAME"
cd "$REPO_ROOT"
git add -A
git commit -m "$(cat <<EOF
exploration: ${TITLE}

Status: ${STATUS}
Baseline: ${BASELINE_MEAN}s
Result: ${RESULT_MEAN}s
Change: ${CHANGE_PCT}%
EOF
)" || log "Nothing to commit on exploration branch"

COMMIT_HASH=$(git rev-parse HEAD)
git push -u origin "$BRANCH_NAME" 2>/dev/null || log "Push failed (no remote, or auth issue). Branch is local only."

# --- step 8: return to main and record results -----------------------------

log "Returning to main branch"
git checkout main

log "Recording results in exploration file"
cat >> "$EXPLORATION_FILE" << RESULTS

## Results

- **Status**: ${STATUS}
- **Commit**: ${COMMIT_HASH} (branch: \`${BRANCH_NAME}\`)
- **Baseline**: ${BASELINE_MEAN}s (+/- ${BASELINE_STDDEV}s)
- **Result**: ${RESULT_MEAN}s (+/- ${RESULT_STDDEV}s)
- **Change**: ${CHANGE_PCT}%
RESULTS

sed -i "s/^pending$/${STATUS}/" "$EXPLORATION_FILE"

git add "$EXPLORATION_FILE"
git commit -m "exploration results: ${SLUG} (${STATUS}, ${CHANGE_PCT}%)"
git tag "exploration-${SLUG}"

log "Done!"
echo ""
echo "  Exploration : $EXPLORATION_FILE"
echo "  Branch      : $BRANCH_NAME"
echo "  Commit      : $COMMIT_HASH"
echo "  Status      : $STATUS"
echo "  Baseline    : ${BASELINE_MEAN}s"
echo "  Result      : ${RESULT_MEAN}s"
echo "  Change      : ${CHANGE_PCT}%"
echo ""
