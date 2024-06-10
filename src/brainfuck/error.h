#pragma once

namespace brainfuck {

enum class Error { NONE = 0, PROGRAM_NOT_FOUND, PROGRAM_TOO_LONG, REACHED_INSRUCTION_LIMIT, UNMATCHED_BRACE };

}
