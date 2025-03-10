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

#include "fboss/agent/gen-cpp2/switch_state_types.h"
#include "fboss/agent/state/BufferPoolConfig.h"
#include "fboss/agent/state/NodeMap.h"
#include "fboss/agent/state/Thrifty.h"
#include "fboss/agent/types.h"

namespace facebook::fboss {

using BufferPoolCfgMapTypeClass = apache::thrift::type_class::map<
    apache::thrift::type_class::string,
    apache::thrift::type_class::structure>;
using BufferPoolCfgMapThriftType =
    std::map<std::string, state::BufferPoolFields>;

class BufferPoolCfgMap;
using BufferPoolCfgMapTraits = ThriftMapNodeTraits<
    BufferPoolCfgMap,
    BufferPoolCfgMapTypeClass,
    BufferPoolCfgMapThriftType,
    BufferPoolCfg>;

/*
 * A container for the set of collectors.
 */
class BufferPoolCfgMap
    : public ThriftMapNode<BufferPoolCfgMap, BufferPoolCfgMapTraits> {
 public:
  using Base = ThriftMapNode<BufferPoolCfgMap, BufferPoolCfgMapTraits>;
  using Traits = BufferPoolCfgMapTraits;
  BufferPoolCfgMap() = default;
  virtual ~BufferPoolCfgMap() override = default;

 private:
  // Inherit the constructors required for clone()
  using Base::Base;
  friend class CloneAllocator;
};

using MultiBufferPoolCfgMapTypeClass = apache::thrift::type_class::
    map<apache::thrift::type_class::string, BufferPoolCfgMapTypeClass>;
using MultiBufferPoolCfgMapThriftType =
    std::map<std::string, BufferPoolCfgMapThriftType>;

class MultiBufferPoolCfgMap;

using MultiBufferPoolCfgMapTraits = ThriftMultiMapNodeTraits<
    MultiBufferPoolCfgMap,
    MultiBufferPoolCfgMapTypeClass,
    MultiBufferPoolCfgMapThriftType,
    BufferPoolCfgMap>;

class HwSwitchMatcher;

class MultiBufferPoolCfgMap
    : public ThriftMapNode<MultiBufferPoolCfgMap, MultiBufferPoolCfgMapTraits> {
 public:
  using Traits = MultiBufferPoolCfgMapTraits;
  using BaseT =
      ThriftMapNode<MultiBufferPoolCfgMap, MultiBufferPoolCfgMapTraits>;
  using BaseT::modify;

  MultiBufferPoolCfgMap() {}
  virtual ~MultiBufferPoolCfgMap() {}

 private:
  // Inherit the constructors required for clone()
  using BaseT::BaseT;
  friend class CloneAllocator;
};

} // namespace facebook::fboss
