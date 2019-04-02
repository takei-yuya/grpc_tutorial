#include <grpcpp/grpcpp.h>

#include "mecab.grpc.pb.h"

int main() {
  std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
  std::unique_ptr<mecab::MeCabServer::Stub> stub = mecab::MeCabServer::NewStub(channel);

  mecab::Query query;
  query.set_document("吾輩は猫であった");

  mecab::MeCabResult result;

  grpc::ClientContext context;

  grpc::Status status = stub->Parse(&context, query, &result);

  if (!status.ok()) exit(EXIT_FAILURE);

  std::cout << result.num_all_tokens() << std::endl;
}
