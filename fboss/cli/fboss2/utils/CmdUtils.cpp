/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "fboss/cli/fboss2/utils/CmdUtils.h"
#include <fboss/agent/if/gen-cpp2/ctrl_types.h>
#include <folly/stop_watch.h>
#include "folly/Conv.h"

#include <folly/logging/LogConfig.h>
#include <folly/logging/LoggerDB.h>
#include <folly/logging/xlog.h>

#include <chrono>
#include <fstream>
#include <string>

using namespace std::chrono;

using folly::ByteRange;
using folly::IPAddress;
using folly::IPAddressV6;

namespace facebook::fboss::utils {

// Converts a readable representation of a link-bandwidth value
//
// bandwidthBytesPerSecond: must be positive number in bytes per second
const std::string formatBandwidth(const float bandwidthBytesPerSecond) {
  if (bandwidthBytesPerSecond < 1.0f) {
    return "Not set";
  }
  const std::string suffixes[] = {"", "K", "M"};
  // Represent the bandwidth in bits per second
  // Use long and floor to ensure that we have integer to start with
  long bandwidthBitsPerSecond = floor(bandwidthBytesPerSecond) * 8;
  for (const auto& suffix : suffixes) {
    if (bandwidthBitsPerSecond < 1000) {
      return folly::to<std::string>(bandwidthBitsPerSecond) + suffix + "bps";
    }
    // we don't round up
    bandwidthBitsPerSecond /= 1000;
  }
  return folly::to<std::string>(bandwidthBitsPerSecond) + "Gbps";
}

/* Takes a list of "friendly" interface names and returns a list of portID
   integers.  This is an operation that is frequently needed so making this
   available as a helper function

   The only way to do this is to get all portInfo from Thrift, compare interface
   name in the lambda function, and if they match add the PortID to the output
   list
*/
std::vector<int32_t> getPortIDList(
    const std::vector<std::string>& ifList,
    std::map<int32_t, facebook::fboss::PortInfoThrift>& portEntries) {
  std::vector<int32_t> portIDList;

  if (ifList.size()) {
    for (auto interface : ifList) {
      auto it = std::find_if(
          portEntries.begin(),
          portEntries.end(),
          [&interface](const auto& port) {
            return port.second.get_name() == interface;
          });
      if (it != portEntries.end()) {
        portIDList.push_back(it->first);
      } else {
        throw std::runtime_error(fmt::format(
            "{} is not a valid interface name on this device", interface));
      }
    }
  }
  return portIDList;
}

std::string getAddrStr(network::thrift::BinaryAddress addr) {
  auto ip = *addr.addr();
  char ipBuff[INET6_ADDRSTRLEN];
  if (ip.size() == 16) {
    inet_ntop(
        AF_INET6,
        &((struct in_addr*)ip.c_str())->s_addr,
        ipBuff,
        INET6_ADDRSTRLEN);
  } else if (ip.size() == 4) {
    inet_ntop(
        AF_INET,
        &((struct in_addr*)ip.c_str())->s_addr,
        ipBuff,
        INET_ADDRSTRLEN);
  } else {
    return "invalid";
  }
  return std::string(ipBuff);
}

std::string getAdminDistanceStr(AdminDistance adminDistance) {
  switch (adminDistance) {
    case AdminDistance::DIRECTLY_CONNECTED:
      return "DIRECTLY_CONNECTED";
    case AdminDistance::STATIC_ROUTE:
      return "STATIC_ROUTE";
    case AdminDistance::OPENR:
      return "OPENR";
    case AdminDistance::EBGP:
      return "EBGP";
    case AdminDistance::IBGP:
      return "IBGP";
    case AdminDistance::MAX_ADMIN_DISTANCE:
      return "MAX_ADMIN_DISTANCE";
  }
  throw std::runtime_error(
      "Unsupported AdminDistance: " +
      std::to_string(static_cast<int>(adminDistance)));
}

const std::string removeFbDomains(const std::string& host) {
  std::string hostCopy = host;
  const RE2 fbDomains(".facebook.com$|.tfbnw.net$");
  RE2::Replace(&hostCopy, fbDomains, "");
  return hostCopy;
}

std::string getSpeedGbps(int64_t speedMbps) {
  return std::to_string(speedMbps / 1000) + "G";
}

std::string getl2EntryTypeStr(L2EntryType l2EntryType) {
  switch (l2EntryType) {
    case L2EntryType::L2_ENTRY_TYPE_PENDING:
      return "Pending";
    case L2EntryType::L2_ENTRY_TYPE_VALIDATED:
      return "Validated";
    default:
      return "Unknown";
  }
}

bool comparePortName(
    const std::basic_string<char>& nameA,
    const std::basic_string<char>& nameB) {
  static const RE2 exp("([a-z][a-z][a-z])(\\d+)/(\\d+)/(\\d)");
  std::string moduleNameA, moduleNumStrA, portNumStrA, subportNumStrA;
  std::string moduleNameB, moduleNumStrB, portNumStrB, subportNumStrB;
  if (!RE2::FullMatch(
          nameA,
          exp,
          &moduleNameA,
          &moduleNumStrA,
          &portNumStrA,
          &subportNumStrA)) {
    throw std::invalid_argument(folly::to<std::string>(
        "Invalid port name: ",
        nameA,
        "\nPort name must match 'moduleNum/port/subport' pattern"));
  }

  if (!RE2::FullMatch(
          nameB,
          exp,
          &moduleNameB,
          &moduleNumStrB,
          &portNumStrB,
          &subportNumStrB)) {
    throw std::invalid_argument(folly::to<std::string>(
        "Invalid port name: ",
        nameB,
        "\nPort name must match 'moduleNum/port/subport' pattern"));
  }

  int ret;
  if ((ret = moduleNameA.compare(moduleNameB)) != 0) {
    return ret < 0;
  }

  if (moduleNumStrA.compare(moduleNumStrB) != 0) {
    return stoi(moduleNumStrA) < stoi(moduleNumStrB);
  }

  if (portNumStrA.compare(portNumStrB) != 0) {
    return stoi(portNumStrA) < stoi(portNumStrB);
  }

  return stoi(subportNumStrA) < stoi(subportNumStrB);
}

bool compareSystemPortName(
    const std::basic_string<char>& nameA,
    const std::basic_string<char>& nameB) {
  static const RE2 exp("([^:]+):([a-z][a-z][a-z])(\\d+)/(\\d+)/(\\d)");
  std::string switchNameA, moduleNameA, moduleNumStrA, portNumStrA,
      subportNumStrA;
  std::string switchNameB, moduleNameB, moduleNumStrB, portNumStrB,
      subportNumStrB;
  if (!RE2::FullMatch(
          nameA,
          exp,
          &switchNameA,
          &moduleNameA,
          &moduleNumStrA,
          &portNumStrA,
          &subportNumStrA)) {
    throw std::invalid_argument(folly::to<std::string>(
        "Invalid port name: ",
        nameA,
        "\nPort name must match 'moduleNum/port/subport' pattern"));
  }

  if (!RE2::FullMatch(
          nameB,
          exp,
          &switchNameB,
          &moduleNameB,
          &moduleNumStrB,
          &portNumStrB,
          &subportNumStrB)) {
    throw std::invalid_argument(folly::to<std::string>(
        "Invalid port name: ",
        nameB,
        "\nPort name must match 'moduleNum/port/subport' pattern"));
  }

  int ret;
  if ((ret = switchNameA.compare(switchNameB)) != 0) {
    return ret < 0;
  }

  if ((ret = moduleNameA.compare(moduleNameB)) != 0) {
    return ret < 0;
  }

  if (moduleNumStrA.compare(moduleNumStrB) != 0) {
    return stoi(moduleNumStrA) < stoi(moduleNumStrB);
  }

  if (portNumStrA.compare(portNumStrB) != 0) {
    return stoi(portNumStrA) < stoi(portNumStrB);
  }

  return stoi(subportNumStrA) < stoi(subportNumStrB);
}
} // namespace facebook::fboss::utils
