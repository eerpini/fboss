/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "fboss/agent/state/SwitchSettings.h"
#include <fboss/agent/gen-cpp2/switch_config_types.h>
#include "common/network/if/gen-cpp2/Address_types.h"
#include "fboss/agent/AddressUtil.h"
#include "fboss/agent/state/SwitchState.h"

#include "fboss/agent/state/NodeBase-defs.h"
#include "folly/dynamic.h"
#include "folly/json.h"

namespace facebook::fboss {

SwitchSettings* SwitchSettings::modify(std::shared_ptr<SwitchState>* state) {
  if (!isPublished()) {
    CHECK(!(*state)->isPublished());
    return this;
  }

  SwitchState::modify(state);
  auto newSwitchSettings = clone();
  auto* ptr = newSwitchSettings.get();
  (*state)->resetSwitchSettings(std::move(newSwitchSettings));
  return ptr;
}

std::unordered_set<SwitchID> SwitchSettings::getSwitchIds() const {
  std::unordered_set<SwitchID> switchIds;
  for (const auto& switchIdAndInfo : getSwitchIdToSwitchInfo()) {
    switchIds.insert(SwitchID(switchIdAndInfo.first));
  }
  return switchIds;
}

std::optional<cfg::SwitchType> SwitchSettings::l3SwitchType() const {
  std::set<cfg::SwitchType> l3SwitchTypes;
  for (const auto& switchIdAndInfo : getSwitchIdToSwitchInfo()) {
    switch (*switchIdAndInfo.second.switchType()) {
      case cfg::SwitchType::NPU:
      case cfg::SwitchType::VOQ:
        l3SwitchTypes.insert(*switchIdAndInfo.second.switchType());
        break;
      case cfg::SwitchType::PHY:
      case cfg::SwitchType::FABRIC:
        break;
    }
  }
  CHECK(l3SwitchTypes.size() <= 1)
      << "Only one type of l3 switch type must be present";
  if (l3SwitchTypes.size()) {
    return *l3SwitchTypes.begin();
  }
  return std::nullopt;
}

template class ThriftStructNode<SwitchSettings, state::SwitchSettingsFields>;

} // namespace facebook::fboss
