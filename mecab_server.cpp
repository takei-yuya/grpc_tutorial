#include <memory>

#include <grpcpp/grpcpp.h>

#include "mecab.grpc.pb.h"

class MeCabServerImpl final : public ::mecab::MeCabServer::Service {
  ::grpc::Status Parse(::grpc::ServerContext* context,
                               const ::mecab::Query* request,
                               ::mecab::MeCabResult* response) override {
    // TODO(takei): impl
    response->set_num_all_tokens(42);
    return ::grpc::Status::OK;
  }
};

int main() {
  MeCabServerImpl mecab_service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
  builder.RegisterService(&mecab_service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();
  return 0;
}
