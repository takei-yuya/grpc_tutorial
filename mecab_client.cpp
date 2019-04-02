#include <grpcpp/grpcpp.h>

#include "mecab.grpc.pb.h"

std::string PosEnum2String(mecab::Query_Pos pos) {
  switch (pos) {
   case mecab::Query::Noun: return "Noun";
   case mecab::Query::Verb: return "Verb";
   case mecab::Query::Adjective: return "Adjective";
  }
  return "Unknwon";
}

int main() {
  std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
  std::unique_ptr<mecab::MeCabServer::Stub> stub = mecab::MeCabServer::NewStub(channel);

  mecab::Query query;
  // TODO(takei): parse args
  query.set_document("吾輩は猫であった");
  query.add_poses(mecab::Query::Noun);

  mecab::MeCabResult result;

  grpc::ClientContext context;

  grpc::Status status = stub->Parse(&context, query, &result);

  if (!status.ok()) exit(EXIT_FAILURE);

  std::cout << "# of all tokens: "<< result.num_all_tokens() << std::endl;
  std::cout << "Target PoS: ";
  for (size_t i = 0, sz = query.poses_size(); i < sz; ++i) {
    if (i != 0) std::cout << ", ";
    std::cout << PosEnum2String(query.poses(i));
  }
  std::cout << std::endl;
  std::cout << "Surface\tbegin\tlength" << std::endl;
  for (size_t i = 0, sz = result.tokens_size(); i < sz; ++i) {
    const mecab::MeCabResult_Token& token = result.tokens(i);
    std::cout << token.surface() << "\t" << token.begin() << "\t" << token.len() << std::endl;
  }
}
