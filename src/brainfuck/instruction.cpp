#include "instruction.h"

brainfuck::Instruction::Instruction(Type type) : type(type), value(0){};
brainfuck::Instruction::Instruction(Type type, Value value) : type(type), value(value){};
brainfuck::Instruction::Instruction(Type type, Value value, Offset offset)
    : type(type), value(value), offset(offset){};  // temp
