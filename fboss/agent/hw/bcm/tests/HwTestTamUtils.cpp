// Copyright 2004-present Facebook. All Rights Reserved.

#include "fboss/agent/hw/test/HwTestTamUtils.h"

#include "fboss/agent/hw/switch_asics/HwAsic.h"
#include "fboss/agent/hw/test/HwSwitchEnsemble.h"

namespace facebook::fboss {
namespace utility {
void triggerParityError(HwSwitchEnsemble* ensemble) {
  std::string out;
  auto asic = ensemble->getPlatform()->getAsic()->getAsicType();
  ensemble->runDiagCommand("\n", out);
  if (asic == cfg::AsicType::ASIC_TYPE_TOMAHAWK4) {
    ensemble->runDiagCommand("ser inject pt=L2_ENTRY_SINGLEm\n", out);
    ensemble->runDiagCommand("ser LOG\n", out);
  } else {
    ensemble->runDiagCommand(
        "ser INJECT memory=L2_ENTRY index=10 pipe=pipe_x\n", out);
    ensemble->runDiagCommand("d chg L2_ENTRY 10 1\n", out);
  }
  ensemble->runDiagCommand("quit\n", out);
  std::ignore = out;
}
} // namespace utility
} // namespace facebook::fboss
