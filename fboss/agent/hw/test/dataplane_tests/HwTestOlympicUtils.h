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
#include "fboss/agent/gen-cpp2/switch_config_types.h"

#include <string>

namespace facebook::fboss {
class HwAsic;
}

/*
 * This utility is to provide utils for hw olympic tests.
 */

namespace facebook::fboss::utility {

enum class OlympicQueueType {
  SILVER,
  GOLD,
  ECN1,
  BRONZE,
  ICP,
  NC,
};

enum class AllSPOlympicQueueType { NCNF, BRONZE, SILVER, GOLD, ICP, NC };

enum class NetworkAIQueueType { MONITORING, RDMA, NC };

/* Olympic QoS queues */
constexpr int kOlympicSilverQueueId = 0;
constexpr int kOlympicGoldQueueId = 1;
constexpr int kOlympicEcn1QueueId = 2;
constexpr int kOlympicBronzeQueueId = 4;
constexpr int kOlympicICPQueueId = 6;
constexpr int kOlympicNCQueueId = 7;

/*
 * Certain ASICs maps higher queue ID to higher priority.
 * Hence queue ID 7 when configured as Strict priority
 * will starve other queues. Certain ASICS (J2) maps in reverse
 * where lower queue ID will be served with higher
 * priority when configured as Strict priority.
 * Defining newer set of queue IDs in the reverse order for:
 * - Olympic Qos
 * - Olympic SP Qos
 * - Network AI QoS
 *
 * This will affect the DSCP to TC QoS map.
 * Eg: For the current platforms, DSCP 48 will be mapped to TC 7
 * and queue 7. Where as in J2 (Or on any DSF BCM platforms),
 * DSCP 48 will be mapped to TC 0 and queue 0.
 *
 * TC -> Queue, TC -> PG, PFC -> PG and PFC -> queue will
 * remain unchanged and will maintain 1 to 1 mapping.
 */
constexpr int kOlympicSilverQueueId2 = 7;
constexpr int kOlympicGoldQueueId2 = 6;
constexpr int kOlympicEcn1QueueId2 = 5;
constexpr int kOlympicBronzeQueueId2 = 3;
constexpr int kOlympicICPQueueId2 = 2;
constexpr int kOlympicNCQueueId2 = 1;

constexpr uint32_t kOlympicSilverWeight = 15;
constexpr uint32_t kOlympicGoldWeight = 80;
constexpr uint32_t kOlympicEcn1Weight = 8;
constexpr uint32_t kOlympicBronzeWeight = 5;

constexpr int kOlympicHighestSPQueueId = kOlympicNCQueueId;

/* Olympic ALL SP QoS queues */
constexpr int kOlympicAllSPNCNFQueueId = 0;
constexpr int kOlympicAllSPBronzeQueueId = 1;
constexpr int kOlympicAllSPSilverQueueId = 2;
constexpr int kOlympicAllSPGoldQueueId = 3;
constexpr int kOlympicAllSPICPQueueId = 6;
constexpr int kOlympicAllSPNCQueueId = 7;

constexpr int kOlympicAllSPNCNFQueueId2 = 7;
constexpr int kOlympicAllSPBronzeQueueId2 = 6;
constexpr int kOlympicAllSPSilverQueueId2 = 5;
constexpr int kOlympicAllSPGoldQueueId2 = 4;
constexpr int kOlympicAllSPICPQueueId2 = 2;
constexpr int kOlympicAllSPNCQueueId2 = 1;

constexpr int kOlympicAllSPHighestSPQueueId = kOlympicAllSPNCQueueId;

/* Queue config params */
constexpr int kQueueConfigBurstSizeMinKb = 1;
constexpr int kQueueConfigBurstSizeMaxKb = 224;
constexpr int kQueueConfigAqmsEcnThresholdMinMax = 120000;
constexpr int kQueueConfigAqmsWredThresholdMinMax = 660000;
constexpr int kQueueConfigAqmsWredDropProbability = 100;

/* network AI Qos queues*/
constexpr int kNetworkAIMonitoringQueueId = 0;
constexpr int kNetworkAIRdmaQueueId = 6;
constexpr int kNetworkAINCQueueId = 7;

constexpr int kNetworkAIMonitoringQueueId2 = 7;
constexpr int kNetworkAIRdmaQueueId2 = 2;
constexpr int kNetworkAINCQueueId2 = 1;

constexpr int kNetworkAIHighestQueueId = kNetworkAINCQueueId;

void addNetworkAIQueueConfig(
    cfg::SwitchConfig* config,
    cfg::StreamType streamType,
    const HwAsic* hwAsic);

void addOlympicQueueConfig(
    cfg::SwitchConfig* config,
    cfg::StreamType streamType,
    const HwAsic* asic,
    bool addWredConfig = false);
void addQueueWredDropConfig(
    cfg::SwitchConfig* config,
    cfg::StreamType streamType,
    const HwAsic* asic);

void addOlympicQosMaps(cfg::SwitchConfig& cfg, const HwAsic* hwAsic);

std::string getOlympicCounterNameForDscp(uint8_t dscp);

const std::map<int, std::vector<uint8_t>> kOlympicQueueToDscp(
    const HwAsic* hwAsic);
const std::map<int, uint8_t> kOlympicWRRQueueToWeight(const HwAsic* hwAsic);

const std::vector<int> kOlympicWRRQueueIds(const HwAsic* hwAsic);
const std::vector<int> kOlympicSPQueueIds(const HwAsic* hwAsic);
const std::vector<int> kOlympicWRRAndICPQueueIds(const HwAsic* hwAsic);
const std::vector<int> kOlympicWRRAndNCQueueIds(const HwAsic* hwAsic);

int getMaxWeightWRRQueue(const std::map<int, uint8_t>& queueToWeight);

void addOlympicAllSPQueueConfig(
    cfg::SwitchConfig* config,
    cfg::StreamType streamType,
    const HwAsic* asic);
void addOlympicAllSPQosMaps(cfg::SwitchConfig& cfg, const HwAsic* asic);

const std::map<int, std::vector<uint8_t>> kOlympicAllSPQueueToDscp(
    const HwAsic* hwAsic);
const std::vector<int> kOlympicAllSPQueueIds(const HwAsic* hwAsic);
cfg::ActiveQueueManagement kGetOlympicEcnConfig(
    int minLength = 41600,
    int maxLength = 41600);
cfg::ActiveQueueManagement kGetWredConfig(
    int minLength = 41600,
    int maxLength = 41600,
    int probability = 100);
void addQueueEcnConfig(
    cfg::SwitchConfig* config,
    const int queueId,
    const uint32_t minLen,
    const uint32_t maxLen);
void addQueueWredConfig(
    cfg::SwitchConfig* config,
    const int queueId,
    const uint32_t minLen,
    const uint32_t maxLen,
    const int probability);
void addQueueShaperConfig(
    cfg::SwitchConfig* config,
    const int queueId,
    const uint32_t minKbps,
    const uint32_t maxKbps);
void addQueueBurstSizeConfig(
    cfg::SwitchConfig* config,
    const int queueId,
    const uint32_t minKbits,
    const uint32_t maxKbits);

int getOlympicQueueId(const HwAsic* hwAsic, OlympicQueueType queueType);

int getOlympicSPQueueId(const HwAsic* hwAsic, AllSPOlympicQueueType queueType);

int getNetworkAIQueueId(const HwAsic* hwAsic, NetworkAIQueueType queueType);

} // namespace facebook::fboss::utility
