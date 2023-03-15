/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include <CLI/CLI.hpp>
#include "fboss/cli/fboss2/utils/CmdCommonUtils.h"

namespace facebook::fboss::utils {

void postAppInit(int /* argc */, char* /* argv */[], CLI::App& /* app */) {}

std::vector<std::string> getHostsInSmcTier(
    const std::string& /* parentTierName */) {
  return {};
}

const std::string getOobNameFromHost(const std::string& /* host */) {
  return "";
}

void logUsage(const CmdLogInfo& /*Cmd Log Info*/) {}

} // namespace facebook::fboss::utils
