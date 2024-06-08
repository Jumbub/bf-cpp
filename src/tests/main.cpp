#define APPROVALS_GOOGLETEST  // This tells Approval Tests to provide a main() - only do this in one cpp file
#include "pch.h"

const auto directoryDisposer = ApprovalTests::Approvals::useApprovalsSubdirectory("snapshots");
const auto frontLoadedReportDisposer =
    ApprovalTests::Approvals::useAsFrontLoadedReporter(ApprovalTests::BlockingReporter::onMachinesNotNamed(""));
