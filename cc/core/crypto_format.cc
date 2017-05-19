// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include "cc/crypto_format.h"
#include "cc/util/errors.h"
#include "cc/util/statusor.h"
#include "proto/tink.pb.h"

using google::cloud::crypto::tink::Keyset;
using google::cloud::crypto::tink::OutputPrefixType;

namespace cloud {
namespace crypto {
namespace tink {

namespace {
// Writes bytes of 'value' in Big Endian order to 'buf'.
// 'buf' must have at least 4 bytes allocated.
void uint32_as_big_endian(uint32_t value, char* buf) {
  buf[0] = 0xff & (value >> 24);
  buf[1] = 0xff & (value >> 16);
  buf[2] = 0xff & (value >> 8);
  buf[3] = 0xff & (value >> 0);
}

}  // anonymous namespace

const int CryptoFormat::kNonRawPrefixSize;
const int CryptoFormat::kLegacyPrefixSize;
const uint8_t CryptoFormat::kLegacyStartByte;

const int CryptoFormat::kTinkPrefixSize;
const uint8_t CryptoFormat::kTinkStartByte;

const int CryptoFormat::kRawPrefixSize;
const std::string CryptoFormat::kRawPrefix = "";

// static
util::StatusOr<std::string> CryptoFormat::get_output_prefix(
    const Keyset::Key& key) {
  switch (key.output_prefix_type()) {
    case OutputPrefixType::TINK: {
      std::string prefix;
      prefix.assign(reinterpret_cast<const char*>(&kTinkStartByte), 1);
      char key_id_buf[4];
      uint32_as_big_endian(key.key_id(), key_id_buf);
      prefix.append(key_id_buf, 4);
      return prefix;
    }
    case OutputPrefixType::LEGACY: {
      std::string prefix;
      prefix.assign(reinterpret_cast<const char*>(&kLegacyStartByte), 1);
      char key_id_buf[4];
      uint32_as_big_endian(key.key_id(), key_id_buf);
      prefix.append(key_id_buf, 4);
      return prefix;
    }
    case OutputPrefixType::RAW:
      return kRawPrefix;
    default:
      return ToStatusF(util::error::INVALID_ARGUMENT,
                       "The given key has invalid OutputPrefixType.");
  }
}

}  // namespace tink
}  // namespace crypto
}  // namespace cloud
