/*
 * Copyright (C) 2019 HERE Europe B.V.
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

#pragma once

#include <mutex>

#include <olp/core/client/HRN.h>
#include <olp/core/client/OlpClient.h>
#include <olp/core/client/OlpClientFactory.h>

#include <olp/dataservice/write/StreamLayerClient.h>
#include "ApiClientLookup.h"
#include "generated/model/Catalog.h"

namespace olp {
namespace client {
class CancellationContext;
class PendingRequests;
}  // namespace client

namespace dataservice {
namespace write {

using InitApiClientsCallback =
    std::function<void(boost::optional<client::ApiError>)>;
using InitCatalogModelCallback =
    std::function<void(boost::optional<client::ApiError>)>;

class StreamLayerClientImpl
    : public std::enable_shared_from_this<StreamLayerClientImpl> {
 public:
  StreamLayerClientImpl(client::HRN catalog,
                        StreamLayerClientSettings client_settings,
                        client::OlpClientSettings settings);

  ~StreamLayerClientImpl();

  bool CancelPendingRequests();

  olp::client::CancellableFuture<PublishDataResponse> PublishData(
      const model::PublishDataRequest& request);
  olp::client::CancellationToken PublishData(
      const model::PublishDataRequest& request, PublishDataCallback callback);

  boost::optional<std::string> Queue(const model::PublishDataRequest& request);
  olp::client::CancellableFuture<StreamLayerClient::FlushResponse> Flush(
      model::FlushRequest request);
  olp::client::CancellationToken Flush(
      model::FlushRequest request, StreamLayerClient::FlushCallback callback);
  size_t QueueSize() const;
  boost::optional<model::PublishDataRequest> PopFromQueue();

  client::CancellableFuture<PublishSdiiResponse> PublishSdii(
      const model::PublishSdiiRequest& request);

  client::CancellationToken PublishSdii(
      const model::PublishSdiiRequest& request, PublishSdiiCallback callback);

 protected:
  virtual PublishSdiiResponse PublishSdiiTask(
      client::HRN catalog, client::OlpClientSettings settings,
      model::PublishSdiiRequest request,
      client::CancellationContext cancellation_context);

  virtual PublishSdiiResponse IngestSDII(client::HRN catalog,
                                         client::CancellationContext,
                                         model::PublishSdiiRequest request,
                                         client::OlpClientSettings settings);

 private:
  client::CancellationToken InitApiClients(
      const std::shared_ptr<client::CancellationContext>& cancel_context,
      InitApiClientsCallback callback);
  client::CancellationToken InitApiClientsGreaterThanTwentyMib(
      const std::shared_ptr<client::CancellationContext>& cancel_context,
      InitApiClientsCallback callback);
  client::CancellationToken InitCatalogModel(
      const model::PublishDataRequest& request,
      const InitCatalogModelCallback& callback);
  void InitPublishDataGreaterThanTwentyMib(
      const std::shared_ptr<client::CancellationContext>& cancel_context,
      const model::PublishDataRequest& request,
      const PublishDataCallback& callback);

  void AquireInitLock();
  void NotifyInitAborted();
  void NotifyInitCompleted();

  std::string FindContentTypeForLayerId(const std::string& layer_id);
  client::CancellationToken PublishDataLessThanTwentyMib(
      const model::PublishDataRequest& request,
      const PublishDataCallback& callback);
  client::CancellationToken PublishDataGreaterThanTwentyMib(
      const model::PublishDataRequest& request,
      const PublishDataCallback& callback);
  std::string GetUuidListKey() const;

 private:
  client::HRN catalog_;
  model::Catalog catalog_model_;

  client::OlpClientSettings settings_;

  std::shared_ptr<client::OlpClient> apiclient_config_;
  std::shared_ptr<client::OlpClient> apiclient_ingest_;
  std::shared_ptr<client::OlpClient> apiclient_blob_;
  std::shared_ptr<client::OlpClient> apiclient_publish_;

  std::mutex init_mutex_;
  std::condition_variable init_cv_;
  bool init_inprogress_;

  std::shared_ptr<cache::KeyValueCache> cache_;
  mutable std::mutex cache_mutex_;
  StreamLayerClientSettings stream_client_settings_;

  std::shared_ptr<client::PendingRequests> pending_requests_;
  std::shared_ptr<thread::TaskScheduler> task_scheduler_;
};

}  // namespace write
}  // namespace dataservice
}  // namespace olp
