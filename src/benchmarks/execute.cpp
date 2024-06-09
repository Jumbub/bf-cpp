#include <functional>
#include <memory>
#include <sstream>
#include "../brainfuck/execute.h"
#include "../brainfuck/file.h"
#include "../brainfuck/parse.h"
#include "benchmark/benchmark.h"
#include "utils.h"

template <class... Args>
void execute(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);
  const std::string filename = std::get<0>(args_tuple);
  const std::string input = std::get<1>(args_tuple);

  const auto code = brainfuck::read(filename);
  if (!code)
    throw std::runtime_error("failed to read code");
  const auto bytecode = brainfuck::parse(code.value());
  if (!bytecode)
    throw std::runtime_error("failed to generate byte code");

  for (auto _ : state) {
    const auto stop = startCapturingIO(input);

    auto output = brainfuck::execute<>(bytecode.value());
    benchmark::DoNotOptimize(output);

    stop();
  }
}

// BENCHMARK_CAPTURE(
//     execute,
//     dbfi_dbfi_hello_world,
//     "samples/dbfi.b",
//     ">>>+[[-]>>[-]++>+>+++++++[<++++>>++<-]++>>+>+>+++++[>++>++++++<<-]+>>>,<++[[>[->>]<[>>]<<-]<[<]<+>>[>]>[<+>-[[<+>-"
//     "]>]<[[[-]<]++<-[<+++++++++>[<->-]>>]>>]]<<]<]<[[<]>[[>]>>[>>]+[<<]<[<]<+>>-]>[>]+[->>]<<<<[[<<]<[<]+<<[+>+<<-[>-->"
//     "+<<-[>+<[>>+<<-]]]>[<+>-]<]++>>-->[>]>>[>>]]<<[>>+<[[<]<]>[[<<]<[<]+[-<+>>-[<<+>++>-[<->[<<+>>-]]]<[>+<-]>]>[>]>]>"
//     "[>>]>>]<<[>>+>>+>>]<<[->>>>>>>>]<<[>.>>>>>>>]<<[>->>>>>]<<[>,>>>]<<[>+>]<<[+<<]<]!--[>--->->->++>-<<<<<-------]>--"
//     ".>---------.>--..+++.>----.>+++++++++.<<.+++.------.<-.>>+.!")
//     ->Unit(benchmark::kSecond);
// BENCHMARK_CAPTURE(execute, mandelbrot, "samples/mandelbrot.b", "")->Unit(benchmark::kSecond);
BENCHMARK_CAPTURE(
    execute,
    cgbfi2_cgbfi2_hello_world,
    "samples/cgbfi2.b",
    ">>>>+[->>>++>+>+++++++[<++++>>++<-]++>>+>+>+++++[>++>++++++<<-]+>>>,<++[[>[->>]<[>>]<<-]<[<]<+>>[>]>[<+>-[[<+>-]>]"
    "<[[[-]<]++<-[<+++++++++>[<->-]>>]>>]]<<]>[-]+<<[--[[-]>>-<<<+>]>>[-<<<<[>+<-]+<<+[->[<+>>>>>>+<<<<<-]<[>+<-]>>>>>>"
    ">+<[-[-[-[-[-[-[-[-[[-]>-<]>[-<<+++++++>>]<]]]>[-]<]>[-<<+++>>]<]>[-<<+>>]<]>[-]<]>[-<<<<<<<+>>>>>>>]<]>[-]<<<<<]>"
    ">>[<<+>>-]<+<[-[-[-[-[-[-[-[-[-[-[[-]>-<<<[-]<<+>>]]]]>[-]<]>[-<<<[-]<<+++++++>>>]<]]]>[-]<]>[-<<<[-]<<+++++++>>>]"
    "<]]>[-]<<<<<<[-]>>[-<<<[>+>>+<<<-]>[<+>-]>>[-[-[[-]>>]>[<<[<+>>+<-]>[<+>-]+<<-[-[-[-[-[-[-[-[-[-[-<+>]<+++++++++++"
    "++>>[-]>->-<<]]]>>[->>>>>]<<]>>[-<<<++++++++++++>>[-]>>-]<<]>>[->>>>>]<<]>>[-<<<+++++++++++>>[-]>>-]<<]>>[-<<<++++"
    "++++++>>[-]>>-]<<]]>>[->>>>>]<<]<]]>[>>]<<<]>>+>>>]<<]>>[->+>]<<<]<<[<<]>>[[<+>>+<-]+<-[-[-[-[-[-[-[-[-[-[-[-[-[-["
    "-[->->>[>>]>>[>>]<[<-<<-<]<[<<]<<[<<]<]>[->>[>>]>>[>>]<[>+>>+>]<[<<]<<[<<]]<]>[->>[>>]>>[>>]<[-]<[<<]<<[<<]]<]>[->"
    ">[>>]>>[>>]<[<-<]<[<<]<<[<<]]<]>[->>[>>]>>[>>]<[>+>]<[<<]<<[<<]]<]>[->>[>>]>>[>>]<<-<<-<<[<<]<<[<<]]<]>[->>[>>]>>["
    ">>]+>>+[<<]<<[<<]]<]>[->>[>>]>>[>>]<++<[<<]<<[<<]]<]>[->>[>>]>>[>>]<+<[<<]<<[<<]]<]>[->>[>>]>>[>>]<,<[<<]<<[<<]]<]"
    ">[->>[>>]>>[>>]<-<[<<]<<[<<]]<]>[->>[>>]>>[>>]<.<[<<]<<[<<]]<]>[->>[>>]>>[>>]<<-<<[<<]<<[<<]]<]>[->>[>>]>>[>>]+[<<"
    "]<<[<<]]<]>[->>[>>]>>[>>]<[>+>>+<<<-]>[<+>-]>>[<<+>>[-]]+<<[>>-<<-]>>[<<+>>>>+<<-]>>[<<+>>-]<<[>>+<<-]+>>[<<->>-]<"
    "<<<[-<<[<<]<<[<<]<<<<<++>>]>>[-<<<<<<[<<]<<[<<]<]>]<]>[->>[>>]>>[>>]<[>+>>+<<<-]>[[<+>-]>>[-]+<<]>>[<<+>>>>+<<-]>>"
    "[<<+>>-]<<[>>+<<-]+>>[<<->>-]<<<<[-<<[<<]<<[<<]<<<<<+>>]>>[-<<<<<<[<<]<<[<<]<]>]>[<+>-]<<<<<<[>>+<<-[->>->>+[>>>[-"
    "<+>>+<]+<-[-[[-]>-<]>[-<<<+>>>]<]>[-<<<->>>]>[-<+>]<<<<[>>+<<-]>>]<<<<<<]>>[-<<+[>>>[-<+>>+<]+<-[-[[-]>-<]>[-<<<->"
    ">>]<]>[-<<<+>>>]>[-<+>]<<<<[<<+>>-]<<]]]>>>>>>>]!--[>--->->->++>-<<<<<-------]>--.>---------.>--..+++.>----.>+++++"
    "++++.<<.+++.------.<-.>>+.!")
    ->Unit(benchmark::kSecond);
BENCHMARK_CAPTURE(
    execute,
    dbfi_hello_world,
    "samples/dbfi.b",
    "--[>--->->->++>-<<<<<-------]>--.>---------.>--..+++.>----.>+++++++++.<<.+++.------.<-.>>+.!");
BENCHMARK_CAPTURE(
    execute,
    cgbfi2_hello_world,
    "samples/cgbfi2.b",
    "--[>--->->->++>-<<<<<-------]>--.>---------.>--..+++.>----.>+++++++++.<<.+++.------.<-.>>+.!");
BENCHMARK_CAPTURE(
    execute,
    cgbfi_hello_world,
    "samples/cgbfi.b",
    "--[>--->->->++>-<<<<<-------]>--.>---------.>--..+++.>----.>+++++++++.<<.+++.------.<-.>>+.!");
BENCHMARK_CAPTURE(execute, rot13, "samples/rot13.b", "~mlk zyx");
BENCHMARK_CAPTURE(execute, hello_world, "samples/hello_world.b", "");
BENCHMARK_CAPTURE(execute, empty_file, "samples/tests/empty_file.b", "");
BENCHMARK_CAPTURE(
    execute,
    echo_lorem_ipsum,
    "samples/echo.b",
    "What is Lorem Ipsum?\n\nLorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum "
    "has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type "
    "and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into "
    "electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of "
    "Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus "
    "PageMaker including versions of Lorem Ipsum.\nWhy do we use it?\n\nIt is a long established fact that a reader "
    "will be distracted by the readable content of a page when looking at its layout. The point of using Lorem Ipsum "
    "is that it has a more-or-less normal distribution of letters, as opposed to using 'Content here, content here', "
    "making it look like readable English. Many desktop publishing packages and web page editors now use Lorem Ipsum "
    "as their default model text, and a search for 'lorem ipsum' will uncover many web sites still in their infancy. "
    "Various versions have evolved over the years, sometimes by accident, sometimes on purpose (injected humour and "
    "the like).\n\nWhere does it come from?\n\nContrary to popular belief, Lorem Ipsum is not simply random text. It "
    "has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, "
    "a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, "
    "consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, "
    "discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of \"de Finibus Bonorum et "
    "Malorum\" (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of "
    "ethics, very popular during the Renaissance. The first line of Lorem Ipsum, \"Lorem ipsum dolor sit amet..\", "
    "comes from a line in section 1.10.32.\n\nThe standard chunk of Lorem Ipsum used since the 1500s is reproduced "
    "below for those interested. Sections 1.10.32 and 1.10.33 from \"de Finibus Bonorum et Malorum\" by Cicero are "
    "also reproduced in their exact original form, accompanied by English versions from the 1914 translation by H. "
    "Rackham.\nWhere can I get some?\n\nThere are many variations of passages of Lorem Ipsum available, but the "
    "majority have suffered alteration in some form, by injected humour, or randomised words which don't look even "
    "slightly believable. If you are going to use a passage of Lorem Ipsum, you need to be sure there isn't anything "
    "embarrassing hidden in the middle of text. All the Lorem Ipsum generators on the Internet tend to repeat "
    "predefined chunks as necessary, making this the first true generator on the Internet. It uses a dictionary of "
    "over 200 Latin words, combined with a handful of model sentence structures, to generate Lorem Ipsum which looks "
    "reasonable. The generated Lorem Ipsum is therefore always free from repetition, injected humour, or "
    "non-characteristic words etc.");
BENCHMARK_CAPTURE(execute, numwarp, "samples/numwarp.b", "()-./0123456789abcdef()-./0123456789abcdef");
BENCHMARK_CAPTURE(execute, _392quine, "samples/392quine.b", "");
BENCHMARK_CAPTURE(execute, bitwidth, "samples/bitwidth.b", "");
BENCHMARK_CAPTURE(
    execute,
    life_glider,
    "samples/life.b",
    "ac\nbc\ncc\ncb\nba\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nq\n");
