// Copyright 2004-present Facebook. All Rights Reserved.

#include "fboss/agent/SwitchInfoTable.h"
#include "fboss/agent/SwSwitch.h"
#include "fboss/agent/hw/switch_asics/HwAsic.h"

#include "fboss/agent/FbossError.h"

namespace facebook::fboss {
SwitchInfoTable::SwitchInfoTable(
    SwSwitch* sw,
    const std::map<int64_t, cfg::SwitchInfo>& switchIdToSwitchInfo)
    : sw_(sw) {
  std::optional<cfg::SwitchType> switchType;
  for (const auto& switchIdAndSwitchInfo : switchIdToSwitchInfo) {
    if (switchType.has_value() &&
        switchType.value() != switchIdAndSwitchInfo.second.switchType()) {
      throw FbossError(
          "Mix of different switch types in SwitchInfoTable is not supported. SwitchTypes ",
          switchType.value(),
          *switchIdAndSwitchInfo.second.switchType());
    } else {
      switchType = *switchIdAndSwitchInfo.second.switchType();
    }
    switchIdToSwitchInfo_.emplace(
        SwitchID(switchIdAndSwitchInfo.first), switchIdAndSwitchInfo.second);
  }
  // TODO - Till SwitchId config is rolled out everywhere
  if (!switchIdToSwitchInfo.size()) {
    cfg::SwitchInfo switchInfo;
    int64_t switchId = sw_->getPlatform()->getAsic()->getSwitchId()
        ? *sw_->getPlatform()->getAsic()->getSwitchId()
        : 0;
    switchInfo.switchType() = sw_->getPlatform()->getAsic()->getSwitchType();
    switchInfo.asicType() = sw_->getPlatform()->getAsic()->getAsicType();
    switchIdToSwitchInfo_.emplace(switchId, switchInfo);
  }
}

std::vector<SwitchID> SwitchInfoTable::getSwitchIdsOfType(
    cfg::SwitchType type) const {
  std::vector<SwitchID> switchIds;
  for (const auto& switchIdAndSwitchInfo : switchIdToSwitchInfo_) {
    if (switchIdAndSwitchInfo.second.switchType() == type) {
      switchIds.emplace_back(switchIdAndSwitchInfo.first);
    }
  }
  return switchIds;
}

std::vector<SwitchID> SwitchInfoTable::getSwitchIDs() const {
  CHECK_NE(switchIdToSwitchInfo_.size(), 0);
  std::vector<SwitchID> switchIds;
  for (const auto& switchIdAndSwitchInfo : switchIdToSwitchInfo_) {
    switchIds.emplace_back(switchIdAndSwitchInfo.first);
  }
  return switchIds;
}

bool SwitchInfoTable::vlansSupported() const {
  for (const auto& switchIdAndSwitchInfo : switchIdToSwitchInfo_) {
    if (switchIdAndSwitchInfo.second.switchType() == cfg::SwitchType::FABRIC ||
        switchIdAndSwitchInfo.second.switchType() == cfg::SwitchType::VOQ) {
      return false;
    }
  }
  return true;
}

cfg::SwitchType SwitchInfoTable::l3SwitchType() const {
  /*
   * We don't allow mixing multiple l3 switch types (since
   * these have different programming models). So look for
   * the first l3 switch type and return that
   */
  if (getSwitchIdsOfType(cfg::SwitchType::NPU).size()) {
    return cfg::SwitchType::NPU;
  }
  if (getSwitchIdsOfType(cfg::SwitchType::VOQ).size()) {
    return cfg::SwitchType::VOQ;
  }
  throw FbossError("No L3 NPUs found");
}
} // namespace facebook::fboss
