#define APPROVALS_GOOGLETEST  // This tells Approval Tests to provide a main() - only do this in one cpp file
#include "ApprovalTests.hpp"

const auto directoryDisposer = ApprovalTests::Approvals::useApprovalsSubdirectory("snapshots");
const auto frontLoadedReportDisposer =
    ApprovalTests::Approvals::useAsFrontLoadedReporter(ApprovalTests::BlockingReporter::onMachinesNotNamed(""));

#include "file.cpp"
#include "go.cpp"
#include "parse.cpp"
