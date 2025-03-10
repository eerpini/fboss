// Copyright 2004-present Facebook. All Rights Reserved.
#include "fboss/util/wedge_qsfp_util.h"
#include "fboss/lib/bsp/BspGenericSystemContainer.h"
#include "fboss/lib/bsp/BspIOBus.h"
#include "fboss/lib/bsp/BspTransceiverApi.h"
#include "fboss/lib/bsp/meru400bfu/Meru400bfuBspPlatformMapping.h"
#include "fboss/lib/bsp/meru400biu/Meru400biuBspPlatformMapping.h"
#include "fboss/lib/bsp/meru800bia/Meru800biaBspPlatformMapping.h"
#include "fboss/lib/platforms/PlatformMode.h"
#include "fboss/lib/platforms/PlatformProductInfo.h"
#include "fboss/lib/usb/GalaxyI2CBus.h"
#include "fboss/lib/usb/Wedge100I2CBus.h"
#include "fboss/lib/usb/WedgeI2CBus.h"

#include <gflags/gflags.h>
#include <sysexits.h>

DEFINE_string(
    platform,
    "",
    "Platform on which we are running."
    " One of (galaxy, wedge100, wedge)");

namespace facebook::fboss {

std::pair<std::unique_ptr<TransceiverI2CApi>, int> getTransceiverAPI() {
  if (FLAGS_platform.size()) {
    if (FLAGS_platform == "galaxy") {
      return std::make_pair(std::make_unique<GalaxyI2CBus>(), 0);
    } else if (FLAGS_platform == "wedge100") {
      return std::make_pair(std::make_unique<Wedge100I2CBus>(), 0);
    } else if (FLAGS_platform == "wedge") {
      return std::make_pair(std::make_unique<WedgeI2CBus>(), 0);
    } else if (FLAGS_platform == "meru400bfu") {
      auto systemContainer =
          BspGenericSystemContainer<Meru400bfuBspPlatformMapping>::getInstance()
              .get();
      auto ioBus = std::make_unique<BspIOBus>(systemContainer);
      return std::make_pair(std::move(ioBus), 0);
    } else if (FLAGS_platform == "meru400biu") {
      auto systemContainer =
          BspGenericSystemContainer<Meru400biuBspPlatformMapping>::getInstance()
              .get();
      auto ioBus = std::make_unique<BspIOBus>(systemContainer);
      return std::make_pair(std::move(ioBus), 0);
    } else if (FLAGS_platform == "meru800bia") {
      auto systemContainer =
          BspGenericSystemContainer<Meru800biaBspPlatformMapping>::getInstance()
              .get();
      auto ioBus = std::make_unique<BspIOBus>(systemContainer);
      return std::make_pair(std::move(ioBus), 0);
    } else {
      fprintf(stderr, "Unknown platform %s\n", FLAGS_platform.c_str());
      return std::make_pair(nullptr, EX_USAGE);
    }
  }

  auto productInfo =
      std::make_unique<PlatformProductInfo>(FLAGS_fruid_filepath);
  productInfo->initialize();

  auto mode = productInfo->getType();
  if (mode == PlatformType::PLATFORM_MERU400BFU) {
    auto systemContainer =
        BspGenericSystemContainer<Meru400bfuBspPlatformMapping>::getInstance()
            .get();
    auto ioBus = std::make_unique<BspIOBus>(systemContainer);
    return std::make_pair(std::move(ioBus), 0);
  } else if (mode == PlatformType::PLATFORM_MERU400BIU) {
    auto systemContainer =
        BspGenericSystemContainer<Meru400biuBspPlatformMapping>::getInstance()
            .get();
    auto ioBus = std::make_unique<BspIOBus>(systemContainer);
    return std::make_pair(std::move(ioBus), 0);
  } else if (mode == PlatformType::PLATFORM_MERU800BIA) {
    auto systemContainer =
        BspGenericSystemContainer<Meru800biaBspPlatformMapping>::getInstance()
            .get();
    auto ioBus = std::make_unique<BspIOBus>(systemContainer);
    return std::make_pair(std::move(ioBus), 0);
  }

  // TODO(klahey):  Should probably verify the other chip architecture.
  if (isTrident2()) {
    return std::make_pair(std::make_unique<WedgeI2CBus>(), 0);
  }
  return std::make_pair(std::make_unique<Wedge100I2CBus>(), 0);
}

/* This function creates and returns the Transceiver Platform Api object
 * for that platform. Cuirrently this is kept empty function and it will
 * be implemented in coming version
 */
std::pair<std::unique_ptr<TransceiverPlatformApi>, int>
getTransceiverPlatformAPI(TransceiverI2CApi* i2cBus) {
  PlatformType mode = PlatformType::PLATFORM_FAKE_WEDGE;

  if (FLAGS_platform.size()) {
    // If the platform is provided by user then use it to create the appropriate
    // Fpga object
    if (FLAGS_platform == "meru400bfu") {
      mode = PlatformType::PLATFORM_MERU400BFU;
    } else if (FLAGS_platform == "meru400biu") {
      mode = PlatformType::PLATFORM_MERU400BIU;
    }
  } else {
    // If the platform is not provided by the user then use current hardware's
    // product info to get the platform type
    auto productInfo =
        std::make_unique<PlatformProductInfo>(FLAGS_fruid_filepath);

    productInfo->initialize();
    mode = productInfo->getType();
  }

  if (mode == PlatformType::PLATFORM_MERU400BFU) {
    auto systemContainer =
        BspGenericSystemContainer<Meru400bfuBspPlatformMapping>::getInstance()
            .get();
    return std::make_pair(
        std::make_unique<BspTransceiverApi>(systemContainer), 0);
  } else if (mode == PlatformType::PLATFORM_MERU400BIU) {
    auto systemContainer =
        BspGenericSystemContainer<Meru400biuBspPlatformMapping>::getInstance()
            .get();
    return std::make_pair(
        std::make_unique<BspTransceiverApi>(systemContainer), 0);
  }
  return std::make_pair(nullptr, EX_USAGE);
}

int getModulesPerController() {
  return 1;
}

} // namespace facebook::fboss
