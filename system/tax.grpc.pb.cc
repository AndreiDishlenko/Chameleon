// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: tax.proto

#include "tax.pb.h"
#include "tax.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace taxprro {

static const char* ChkIncomeService_method_names[] = {
  "/taxprro.ChkIncomeService/sendChk",
  "/taxprro.ChkIncomeService/sendChkV2",
  "/taxprro.ChkIncomeService/lastChk",
  "/taxprro.ChkIncomeService/ping",
  "/taxprro.ChkIncomeService/delLastChk",
  "/taxprro.ChkIncomeService/delLastChkId",
  "/taxprro.ChkIncomeService/statusRro",
  "/taxprro.ChkIncomeService/infoRro",
};

std::unique_ptr< ChkIncomeService::Stub> ChkIncomeService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< ChkIncomeService::Stub> stub(new ChkIncomeService::Stub(channel, options));
  return stub;
}

ChkIncomeService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_sendChk_(ChkIncomeService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_sendChkV2_(ChkIncomeService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_lastChk_(ChkIncomeService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ping_(ChkIncomeService_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_delLastChk_(ChkIncomeService_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_delLastChkId_(ChkIncomeService_method_names[5], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_statusRro_(ChkIncomeService_method_names[6], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_infoRro_(ChkIncomeService_method_names[7], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status ChkIncomeService::Stub::sendChk(::grpc::ClientContext* context, const ::taxprro::Check& request, ::taxprro::CheckResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_sendChk_, context, request, response);
}

void ChkIncomeService::Stub::async::sendChk(::grpc::ClientContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_sendChk_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::sendChk(::grpc::ClientContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_sendChk_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::PrepareAsyncsendChkRaw(::grpc::ClientContext* context, const ::taxprro::Check& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::CheckResponse, ::taxprro::Check, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_sendChk_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::AsyncsendChkRaw(::grpc::ClientContext* context, const ::taxprro::Check& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncsendChkRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status ChkIncomeService::Stub::sendChkV2(::grpc::ClientContext* context, const ::taxprro::Check& request, ::taxprro::CheckResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_sendChkV2_, context, request, response);
}

void ChkIncomeService::Stub::async::sendChkV2(::grpc::ClientContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_sendChkV2_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::sendChkV2(::grpc::ClientContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_sendChkV2_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::PrepareAsyncsendChkV2Raw(::grpc::ClientContext* context, const ::taxprro::Check& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::CheckResponse, ::taxprro::Check, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_sendChkV2_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::AsyncsendChkV2Raw(::grpc::ClientContext* context, const ::taxprro::Check& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncsendChkV2Raw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status ChkIncomeService::Stub::lastChk(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::taxprro::CheckResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::CheckRequest, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_lastChk_, context, request, response);
}

void ChkIncomeService::Stub::async::lastChk(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::CheckResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::CheckRequest, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_lastChk_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::lastChk(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::CheckResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_lastChk_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::PrepareAsynclastChkRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::CheckResponse, ::taxprro::CheckRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_lastChk_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::AsynclastChkRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsynclastChkRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status ChkIncomeService::Stub::ping(::grpc::ClientContext* context, const ::taxprro::Check& request, ::taxprro::CheckResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_ping_, context, request, response);
}

void ChkIncomeService::Stub::async::ping(::grpc::ClientContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ping_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::ping(::grpc::ClientContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ping_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::PrepareAsyncpingRaw(::grpc::ClientContext* context, const ::taxprro::Check& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::CheckResponse, ::taxprro::Check, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_ping_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::AsyncpingRaw(::grpc::ClientContext* context, const ::taxprro::Check& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncpingRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status ChkIncomeService::Stub::delLastChk(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::taxprro::CheckResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::CheckRequest, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_delLastChk_, context, request, response);
}

void ChkIncomeService::Stub::async::delLastChk(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::CheckResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::CheckRequest, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_delLastChk_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::delLastChk(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::CheckResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_delLastChk_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::PrepareAsyncdelLastChkRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::CheckResponse, ::taxprro::CheckRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_delLastChk_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::AsyncdelLastChkRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncdelLastChkRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status ChkIncomeService::Stub::delLastChkId(::grpc::ClientContext* context, const ::taxprro::CheckRequestId& request, ::taxprro::CheckResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::CheckRequestId, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_delLastChkId_, context, request, response);
}

void ChkIncomeService::Stub::async::delLastChkId(::grpc::ClientContext* context, const ::taxprro::CheckRequestId* request, ::taxprro::CheckResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::CheckRequestId, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_delLastChkId_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::delLastChkId(::grpc::ClientContext* context, const ::taxprro::CheckRequestId* request, ::taxprro::CheckResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_delLastChkId_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::PrepareAsyncdelLastChkIdRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequestId& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::CheckResponse, ::taxprro::CheckRequestId, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_delLastChkId_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::CheckResponse>* ChkIncomeService::Stub::AsyncdelLastChkIdRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequestId& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncdelLastChkIdRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status ChkIncomeService::Stub::statusRro(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::taxprro::StatusResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::CheckRequest, ::taxprro::StatusResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_statusRro_, context, request, response);
}

void ChkIncomeService::Stub::async::statusRro(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::StatusResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::CheckRequest, ::taxprro::StatusResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_statusRro_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::statusRro(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::StatusResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_statusRro_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::StatusResponse>* ChkIncomeService::Stub::PrepareAsyncstatusRroRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::StatusResponse, ::taxprro::CheckRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_statusRro_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::StatusResponse>* ChkIncomeService::Stub::AsyncstatusRroRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncstatusRroRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status ChkIncomeService::Stub::infoRro(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::taxprro::RroInfoResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::taxprro::CheckRequest, ::taxprro::RroInfoResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_infoRro_, context, request, response);
}

void ChkIncomeService::Stub::async::infoRro(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::RroInfoResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::taxprro::CheckRequest, ::taxprro::RroInfoResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_infoRro_, context, request, response, std::move(f));
}

void ChkIncomeService::Stub::async::infoRro(::grpc::ClientContext* context, const ::taxprro::CheckRequest* request, ::taxprro::RroInfoResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_infoRro_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::taxprro::RroInfoResponse>* ChkIncomeService::Stub::PrepareAsyncinfoRroRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::taxprro::RroInfoResponse, ::taxprro::CheckRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_infoRro_, context, request);
}

::grpc::ClientAsyncResponseReader< ::taxprro::RroInfoResponse>* ChkIncomeService::Stub::AsyncinfoRroRaw(::grpc::ClientContext* context, const ::taxprro::CheckRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncinfoRroRaw(context, request, cq);
  result->StartCall();
  return result;
}

ChkIncomeService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::Check* req,
             ::taxprro::CheckResponse* resp) {
               return service->sendChk(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::Check* req,
             ::taxprro::CheckResponse* resp) {
               return service->sendChkV2(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::CheckRequest, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::CheckRequest* req,
             ::taxprro::CheckResponse* resp) {
               return service->lastChk(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::Check, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::Check* req,
             ::taxprro::CheckResponse* resp) {
               return service->ping(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::CheckRequest, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::CheckRequest* req,
             ::taxprro::CheckResponse* resp) {
               return service->delLastChk(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::CheckRequestId, ::taxprro::CheckResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::CheckRequestId* req,
             ::taxprro::CheckResponse* resp) {
               return service->delLastChkId(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[6],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::CheckRequest, ::taxprro::StatusResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::CheckRequest* req,
             ::taxprro::StatusResponse* resp) {
               return service->statusRro(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      ChkIncomeService_method_names[7],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< ChkIncomeService::Service, ::taxprro::CheckRequest, ::taxprro::RroInfoResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](ChkIncomeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::taxprro::CheckRequest* req,
             ::taxprro::RroInfoResponse* resp) {
               return service->infoRro(ctx, req, resp);
             }, this)));
}

ChkIncomeService::Service::~Service() {
}

::grpc::Status ChkIncomeService::Service::sendChk(::grpc::ServerContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ChkIncomeService::Service::sendChkV2(::grpc::ServerContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ChkIncomeService::Service::lastChk(::grpc::ServerContext* context, const ::taxprro::CheckRequest* request, ::taxprro::CheckResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ChkIncomeService::Service::ping(::grpc::ServerContext* context, const ::taxprro::Check* request, ::taxprro::CheckResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ChkIncomeService::Service::delLastChk(::grpc::ServerContext* context, const ::taxprro::CheckRequest* request, ::taxprro::CheckResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ChkIncomeService::Service::delLastChkId(::grpc::ServerContext* context, const ::taxprro::CheckRequestId* request, ::taxprro::CheckResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ChkIncomeService::Service::statusRro(::grpc::ServerContext* context, const ::taxprro::CheckRequest* request, ::taxprro::StatusResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status ChkIncomeService::Service::infoRro(::grpc::ServerContext* context, const ::taxprro::CheckRequest* request, ::taxprro::RroInfoResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace taxprro

