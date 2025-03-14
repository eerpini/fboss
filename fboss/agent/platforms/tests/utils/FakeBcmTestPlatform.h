/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include <folly/experimental/TestUtil.h>

#include "fboss/agent/platforms/tests/utils/BcmTestPlatform.h"

namespace facebook::fboss {

class FakeAsic;

class FakeBcmTestPlatform : public BcmTestPlatform {
 public:
  FakeBcmTestPlatform();
  ~FakeBcmTestPlatform() override;

  std::vector<FlexPortMode> getSupportedFlexPortModes() const override {
    return {
        FlexPortMode::ONEX100G,
        FlexPortMode::TWOX50G,
        FlexPortMode::ONEX40G,
        FlexPortMode::FOURX25G,
        FlexPortMode::FOURX10G};
  }

  std::string getVolatileStateDir() const override;
  std::string getPersistentStateDir() const override;
  bool hasLinkScanCapability() const override {
    return false;
  }
  bool isBcmShellSupported() const override {
    return false;
  }
  const PortQueue& getDefaultPortQueueSettings(
      cfg::StreamType streamType) const override;
  const PortQueue& getDefaultControlPlaneQueueSettings(
      cfg::StreamType streamType) const override;
  const PortPgConfig& getDefaultPortPgSettings() const override;
  const BufferPoolCfg& getDefaultPortIngressPoolSettings() const override;

  uint32_t getMMUBufferBytes() const override {
    return 16 * 1024 * 1024;
  }
  uint32_t getMMUCellBytes() const override {
    return 208;
  }

  bool useQueueGportForCos() const override {
    return true;
  }

  HwAsic* getAsic() const override;

 private:
  void setupAsic(
      cfg::SwitchType switchType,
      std::optional<int64_t> switchId,
      std::optional<cfg::Range64> systemPortRange) override;
  // Forbidden copy constructor and assignment operator
  FakeBcmTestPlatform(FakeBcmTestPlatform const&) = delete;
  FakeBcmTestPlatform& operator=(FakeBcmTestPlatform const&) = delete;

  std::unique_ptr<BcmTestPort> createTestPort(PortID id) override;

  folly::test::TemporaryDirectory tmpDir_;
  std::unique_ptr<FakeAsic> asic_;
};

} // namespace facebook::fboss
