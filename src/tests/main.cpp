#define APPROVALS_GOOGLETEST  // This tells Approval Tests to provide a main() - only do this in one cpp file
#include "ApprovalTests.hpp"

#include <iostream>
#include "../brainfuck/file.h"
#include "../brainfuck/go.h"
#include "../brainfuck/parse.h"

auto directoryDisposer = ApprovalTests::Approvals::useApprovalsSubdirectory("snapshots");
auto frontLoadedReportDisposer =
    ApprovalTests::Approvals::useAsFrontLoadedReporter(ApprovalTests::BlockingReporter::onMachinesNotNamed(""));

#include "file.cpp"
#include "parse.cpp"
