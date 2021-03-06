/*
 * Copyright (C) 2020 HERE Europe B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */

#include <gtest/gtest.h>

#include <cache/DefaultCacheImpl.h>
#include <olp/core/utils/Dir.h>

using namespace olp::cache;

namespace {
class DefaultCacheImplHelper : public DefaultCacheImpl {
 public:
  DefaultCacheImplHelper(const CacheSettings& settings)
      : DefaultCacheImpl(settings){};

  bool HasLruCache() const { return GetMutableCacheLru().get() != nullptr; }

  bool ContainsLru(const std::string& key) const {
    const auto& lru_cache = GetMutableCacheLru();
    if (!lru_cache) {
      return false;
    }

    return lru_cache->FindNoPromote(key) != lru_cache->end();
  }

  bool ContainsMemoryCache(const std::string& key) const {
    const auto& memory_cache = GetMemoryCache();
    if (!memory_cache) {
      return false;
    }

    return !memory_cache->Get(key).empty();
  }

  bool ContainsMutableCache(const std::string& key) const {
    const auto& disk_cache = GetMutableCache();
    if (!disk_cache) {
      return false;
    }

    return disk_cache->Get(key) != boost::none;
  }

  uint64_t Size() const { return GetMutableCacheSize(); }

  uint64_t CalculateExpirySize(const std::string& key) const {
    const auto expiry_key = GetExpiryKey(key);
    const auto& disk_cache = GetMutableCache();
    if (!disk_cache) {
      return 0u;
    }

    const auto expiry_str = disk_cache->Get(expiry_key).get_value_or({});
    if (expiry_str.empty()) {
      return 0u;
    }

    return expiry_key.size() + expiry_str.size();
  }

  DiskLruCache::const_iterator BeginLru() {
    const auto& lru_cache = GetMutableCacheLru();
    if (!lru_cache) {
      return DiskLruCache::const_iterator{};
    }

    return lru_cache->begin();
  }

  DiskLruCache::const_iterator EndLru() {
    const auto& lru_cache = GetMutableCacheLru();
    if (!lru_cache) {
      return DiskLruCache::const_iterator{};
    }

    return lru_cache->end();
  }
};

TEST(DefaultCacheImplTest, LruCache) {
  std::string cache_path = olp::utils::Dir::TempDirectory() + "/unittest";

  {
    SCOPED_TRACE("Successful creation");

    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    DefaultCacheImplHelper cache(settings);
    cache.Open();

    EXPECT_TRUE(cache.HasLruCache());
  }

  {
    SCOPED_TRACE("kLeastRecentlyUsed eviction policy");

    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    settings.eviction_policy = EvictionPolicy::kLeastRecentlyUsed;
    DefaultCacheImplHelper cache(settings);
    cache.Open();

    EXPECT_TRUE(cache.HasLruCache());
  }

  {
    SCOPED_TRACE("Close");

    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Close();

    EXPECT_FALSE(cache.HasLruCache());
  }

  {
    SCOPED_TRACE("No Open() call");

    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    DefaultCacheImplHelper cache(settings);

    EXPECT_FALSE(cache.HasLruCache());
  }

  {
    SCOPED_TRACE("Default settings");

    olp::cache::CacheSettings settings;
    DefaultCacheImplHelper cache(settings);
    cache.Open();

    EXPECT_FALSE(cache.HasLruCache());
  }

  {
    SCOPED_TRACE("No disk cache size limit");

    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    settings.max_disk_storage = std::uint64_t(-1);
    DefaultCacheImplHelper cache(settings);
    cache.Open();

    EXPECT_FALSE(cache.HasLruCache());
  }

  {
    SCOPED_TRACE("kNone eviction policy");

    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    settings.eviction_policy = EvictionPolicy::kNone;
    DefaultCacheImplHelper cache(settings);
    cache.Open();

    EXPECT_FALSE(cache.HasLruCache());
  }
}

TEST(DefaultCacheImplTest, LruCachePut) {
  olp::cache::CacheSettings settings;
  settings.disk_path_mutable = olp::utils::Dir::TempDirectory() + "/unittest";

  {
    SCOPED_TRACE("Put decode");

    constexpr auto data_string{"this is key's data"};
    constexpr auto key{"somekey"};
    DefaultCacheImplHelper cache(settings);

    cache.Open();
    cache.Clear();
    cache.Put(key, data_string, [=]() { return data_string; },
              (std::numeric_limits<time_t>::max)());

    EXPECT_TRUE(cache.ContainsLru(key));
  }

  {
    SCOPED_TRACE("Put binary");

    std::vector<unsigned char> binary_data = {1, 2, 3};
    constexpr auto key{"somekey"};
    DefaultCacheImplHelper cache(settings);

    cache.Open();
    cache.Clear();
    cache.Put(key, std::make_shared<std::vector<unsigned char>>(binary_data),
              (std::numeric_limits<time_t>::max)());

    EXPECT_TRUE(cache.ContainsLru(key));
  }
}

TEST(DefaultCacheImplTest, LruCacheGetPromote) {
  olp::cache::CacheSettings settings;
  settings.disk_path_mutable = olp::utils::Dir::TempDirectory() + "/unittest";
  constexpr auto key1{"somekey1"};
  constexpr auto key2{"somekey2"};
  constexpr auto data_string{"this is key's data"};
  std::vector<unsigned char> binary_data = {1, 2, 3};

  DefaultCacheImplHelper cache(settings);

  cache.Open();
  cache.Clear();
  cache.Put(key1, data_string, [=]() { return data_string; },
            (std::numeric_limits<time_t>::max)());
  cache.Put(key2, std::make_shared<std::vector<unsigned char>>(binary_data),
            (std::numeric_limits<time_t>::max)());

  {
    SCOPED_TRACE("Get decode promote");

    cache.Get(key1, [=](const std::string&) { return data_string; });
    auto it = cache.BeginLru();

    EXPECT_EQ(key1, it->key());
  }

  {
    SCOPED_TRACE("Get binary promote");

    cache.Get(key2);
    auto it = cache.BeginLru();

    EXPECT_EQ(key2, it->key());
  }
}

TEST(DefaultCacheImplTest, LruCacheRemove) {
  olp::cache::CacheSettings settings;
  settings.disk_path_mutable = olp::utils::Dir::TempDirectory() + "/unittest";
  constexpr auto key1{"somekey1"};
  constexpr auto key2{"somekey2"};
  constexpr auto key3{"anotherkey1"};
  constexpr auto invalid_key{"invalid"};
  constexpr auto data_string{"this is key's data"};
  std::vector<unsigned char> binary_data = {1, 2, 3};

  {
    SCOPED_TRACE("Remove from cache");

    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    bool put_result = cache.Put(
        key1, std::make_shared<std::vector<unsigned char>>(binary_data),
        (std::numeric_limits<time_t>::max)());
    ASSERT_TRUE(put_result);

    auto data_read = cache.Get(key1);

    ASSERT_TRUE(data_read);
    EXPECT_EQ(*data_read, binary_data);

    // check removing missing key is not an error.
    EXPECT_TRUE(cache.Remove(invalid_key));
    EXPECT_TRUE(cache.Remove(key1));
    EXPECT_FALSE(cache.ContainsLru(key1));
  }

  {
    SCOPED_TRACE("RemoveWithPrefix");

    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    cache.Put(key1, std::make_shared<std::vector<unsigned char>>(binary_data),
              (std::numeric_limits<time_t>::max)());
    cache.Put(key2, std::make_shared<std::vector<unsigned char>>(binary_data),
              (std::numeric_limits<time_t>::max)());
    cache.Put(key3, data_string, [=]() { return data_string; },
              (std::numeric_limits<time_t>::max)());

    EXPECT_TRUE(cache.ContainsLru(key1));
    EXPECT_TRUE(cache.ContainsLru(key2));
    EXPECT_TRUE(cache.ContainsLru(key3));

    auto result = cache.RemoveKeysWithPrefix(invalid_key);

    ASSERT_TRUE(result);
    EXPECT_TRUE(cache.ContainsLru(key1));
    EXPECT_TRUE(cache.ContainsLru(key2));
    EXPECT_TRUE(cache.ContainsLru(key3));

    result = cache.RemoveKeysWithPrefix("another");

    ASSERT_TRUE(result);
    EXPECT_TRUE(cache.ContainsLru(key1));
    EXPECT_TRUE(cache.ContainsLru(key2));
    EXPECT_FALSE(cache.ContainsLru(key3));

    result = cache.RemoveKeysWithPrefix("some");

    ASSERT_TRUE(result);
    EXPECT_FALSE(cache.ContainsLru(key1));
    EXPECT_FALSE(cache.ContainsLru(key2));
  }
}

TEST(DefaultCacheImplTest, MutableCacheSize) {
  const std::string key1{"somekey1"};
  const std::string key2{"somekey2"};
  const std::string key3{"anotherkey1"};
  const std::string invalid_key{"invalid"};
  const std::string data_string{"this is key's data"};
  constexpr auto expiry = 321;
  std::vector<unsigned char> binary_data = {1, 2, 3};
  const auto data_ptr =
      std::make_shared<std::vector<unsigned char>>(binary_data);

  std::string cache_path = olp::utils::Dir::TempDirectory() + "/unittest";
  olp::cache::CacheSettings settings;
  settings.disk_path_mutable = cache_path;

  {
    SCOPED_TRACE("Put decode");

    settings.eviction_policy = EvictionPolicy::kNone;
    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    cache.Put(
        key1, data_string, [=]() { return data_string; },
        (std::numeric_limits<time_t>::max)());
    auto data_size = key1.size() + data_string.size();

    EXPECT_EQ(data_size, cache.Size());

    cache.Put(
        key2, data_string, [=]() { return data_string; }, expiry);
    data_size +=
        key2.size() + data_string.size() + cache.CalculateExpirySize(key2);

    EXPECT_EQ(data_size, cache.Size());
  }

  {
    SCOPED_TRACE("Put binary");

    settings.eviction_policy = EvictionPolicy::kNone;
    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    cache.Put(key1, data_ptr, (std::numeric_limits<time_t>::max)());
    auto data_size = key1.size() + binary_data.size();

    EXPECT_EQ(data_size, cache.Size());

    cache.Put(key2, data_ptr, expiry);
    data_size +=
        key2.size() + binary_data.size() + cache.CalculateExpirySize(key2);

    EXPECT_EQ(data_size, cache.Size());
  }

  {
    SCOPED_TRACE("Remove from cache");

    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    cache.Put(key1, data_ptr, (std::numeric_limits<time_t>::max)());
    cache.Put(
        key2, data_string, [=]() { return data_string; },
        (std::numeric_limits<time_t>::max)());

    cache.Remove(key1);
    cache.Remove(key2);
    cache.Remove(invalid_key);

    EXPECT_EQ(0u, cache.Size());
  }

  {
    SCOPED_TRACE("Remove from cache with expiry");

    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    cache.Put(key1, data_ptr, expiry);
    cache.Put(
        key2, data_string, [=]() { return data_string; }, expiry);

    cache.Remove(key1);
    cache.Remove(key2);
    cache.Remove(invalid_key);

    EXPECT_EQ(0u, cache.Size());
  }

  {
    SCOPED_TRACE("RemoveWithPrefix");

    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    cache.Put(key1, data_ptr, (std::numeric_limits<time_t>::max)());
    cache.Put(key2, data_ptr, expiry);
    cache.Put(
        key3, data_string, [=]() { return data_string; }, expiry);
    const auto data_size =
        key3.size() + data_string.size() + cache.CalculateExpirySize(key3);

    cache.RemoveKeysWithPrefix(invalid_key);
    cache.RemoveKeysWithPrefix("some");

    EXPECT_EQ(data_size, cache.Size());
  }

  {
    SCOPED_TRACE("Expiry");

    DefaultCacheImplHelper cache(settings);
    cache.Open();
    cache.Clear();

    cache.Put(key1, data_ptr, -1);
    cache.Put(
        key2, data_string, [=]() { return data_string; }, -1);

    cache.Get(key1);
    cache.Get(key2, [](const std::string& value) { return value; });

    EXPECT_EQ(0, cache.Size());
  }

  {
    SCOPED_TRACE("Clear/close/open");
    const std::string data_string{"this is key's data"};
    const std::string key{"somekey"};

    settings.eviction_policy = EvictionPolicy::kNone;
    DefaultCacheImplHelper cache(settings);

    cache.Open();
    cache.Clear();

    cache.Put(
        key, data_string, [=]() { return data_string; },
        (std::numeric_limits<time_t>::max)());
    const auto data_size = key.size() + data_string.size();
    cache.Close();
    EXPECT_EQ(0u, cache.Size());

    cache.Open();
    EXPECT_EQ(data_size, cache.Size());

    cache.Clear();
    EXPECT_EQ(0u, cache.Size());
  }

  {
    SCOPED_TRACE("Cache not blocked");

    const auto prefix = "somekey";
    const auto data_size = 1024u;
    std::vector<unsigned char> binary_data(data_size);
    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    settings.eviction_policy = EvictionPolicy::kNone;
    settings.max_disk_storage = 2u * 1024u * 1024u;
    DefaultCacheImplHelper cache(settings);

    cache.Open();
    cache.Clear();

    // fill the cache till it full
    auto count = 0u;
    const auto max_count = settings.max_disk_storage / data_size;
    auto total_size = 0u;
    for (; count < max_count; ++count) {
      const auto key = prefix + std::to_string(count);
      const auto elem_size = key.size() + binary_data.size();
      if (total_size + elem_size > settings.max_disk_storage) {
        break;
      }

      const auto result = cache.Put(
          key, std::make_shared<std::vector<unsigned char>>(binary_data),
          (std::numeric_limits<time_t>::max)());
      total_size += elem_size;
      ASSERT_TRUE(result);
    }

    auto result =
        cache.Put(prefix + std::to_string(count),
                  std::make_shared<std::vector<unsigned char>>(binary_data),
                  (std::numeric_limits<time_t>::max)());
    EXPECT_FALSE(result);
    EXPECT_TRUE(total_size < settings.max_disk_storage);
    EXPECT_EQ(total_size, cache.Size());

    cache.Remove(prefix + std::to_string(count - 1));
    cache.Remove(prefix + std::to_string(count - 2));
    result =
        cache.Put(prefix + std::to_string(count),
                  std::make_shared<std::vector<unsigned char>>(binary_data),
                  (std::numeric_limits<time_t>::max)());

    EXPECT_TRUE(result);
    EXPECT_TRUE(total_size > cache.Size());
  }
}

TEST(DefaultCacheImplTest, LruCacheEviction) {
  std::string cache_path = olp::utils::Dir::TempDirectory() + "/unittest";

  {
    SCOPED_TRACE("kNone evicts nothing");

    const auto prefix = "somekey";
    const auto data_size = 1024u;
    std::vector<unsigned char> binary_data(data_size);
    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    settings.eviction_policy = EvictionPolicy::kNone;
    settings.max_disk_storage = 2u * 1024u * 1024u;
    DefaultCacheImplHelper cache(settings);

    cache.Open();
    cache.Clear();

    // fill the cache till it full
    auto count = 0u;
    const auto max_count = settings.max_disk_storage / data_size;
    for (; count < max_count; ++count) {
      const auto key = prefix + std::to_string(count);
      const auto result = cache.Put(
          key, std::make_shared<std::vector<unsigned char>>(binary_data),
          (std::numeric_limits<time_t>::max)());
      if (!result) {
        break;
      }

      EXPECT_TRUE(cache.ContainsMutableCache(key));
      EXPECT_TRUE(cache.ContainsMemoryCache(key));
    }

    // maximum is reached = cache to full.
    ASSERT_FALSE(count == max_count);
    EXPECT_FALSE(cache.HasLruCache());

    // check all data is in the cache.
    for (auto i = 0u; i < count; ++i) {
      const auto key = prefix + std::to_string(i);
      const auto value = cache.Get(key);

      EXPECT_TRUE(value.get() != nullptr);
      EXPECT_TRUE(cache.ContainsMutableCache(key));
      EXPECT_TRUE(cache.ContainsMemoryCache(key));
    }
    cache.Clear();
  }

  {
    SCOPED_TRACE("kLeastRecentlyUsed eviction");

    const auto prefix{"somekey"};
    const auto data_size = 1024u;
    std::vector<unsigned char> binary_data(data_size);
    olp::cache::CacheSettings settings;
    settings.disk_path_mutable = cache_path;
    settings.eviction_policy = EvictionPolicy::kLeastRecentlyUsed;
    settings.max_disk_storage = 2u * 1024u * 1024u;
    DefaultCacheImplHelper cache(settings);

    cache.Open();
    cache.Clear();

    const auto promote_key = prefix + std::to_string(0);
    const auto evicted_key = prefix + std::to_string(1);
    cache.Put(promote_key,
              std::make_shared<std::vector<unsigned char>>(binary_data),
              (std::numeric_limits<time_t>::max)());

    // overflow the mutable cache
    auto count = 0u;
    std::string key;
    const auto max_count = settings.max_disk_storage / data_size;
    for (; count < max_count; ++count) {
      key = prefix + std::to_string(count);
      const auto result = cache.Put(
          key, std::make_shared<std::vector<unsigned char>>(binary_data),
          (std::numeric_limits<time_t>::max)());

      ASSERT_TRUE(result);

      // promote the key so its not evicted.
      cache.Get(promote_key);

      EXPECT_TRUE(cache.ContainsMutableCache(key));
      EXPECT_TRUE(cache.ContainsMemoryCache(key));
      EXPECT_TRUE(cache.ContainsLru(key));
      EXPECT_TRUE(cache.ContainsMutableCache(promote_key));
      EXPECT_TRUE(cache.ContainsMemoryCache(promote_key));
      EXPECT_TRUE(cache.ContainsLru(promote_key));
    }
    const auto promote_value = cache.Get(promote_key);
    const auto value = cache.Get(key);

    // maximum is reached.
    ASSERT_TRUE(count == max_count);
    EXPECT_TRUE(cache.HasLruCache());
    EXPECT_TRUE(value.get() != nullptr);
    EXPECT_TRUE(promote_value.get() != nullptr);

    EXPECT_TRUE(cache.ContainsMutableCache(promote_key));
    EXPECT_TRUE(cache.ContainsMemoryCache(promote_key));
    EXPECT_TRUE(cache.ContainsLru(promote_key));
    EXPECT_TRUE(cache.ContainsMutableCache(key));
    EXPECT_TRUE(cache.ContainsMemoryCache(key));
    EXPECT_TRUE(cache.ContainsLru(key));

    // some items are removed, because eviction starts before the cache is full
    EXPECT_FALSE(cache.ContainsMutableCache(evicted_key));
    EXPECT_FALSE(cache.ContainsMemoryCache(evicted_key));
    EXPECT_FALSE(cache.ContainsLru(evicted_key));
    cache.Clear();
  }
}

}  // namespace
