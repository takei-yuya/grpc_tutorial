#include <memory>

#include <grpcpp/grpcpp.h>

#include "mecab.grpc.pb.h"
#include "mecab.h"

class MeCabServerImpl final : public ::mecab::MeCabServer::Service {
  ::grpc::Status Parse(::grpc::ServerContext* context,
                               const ::mecab::Query* request,
                               ::mecab::MeCabResult* response) override {
    std::unordered_set<std::string> poses;
    for (size_t i = 0, sz = request->poses_size(); i < sz; ++i) {
      switch (request->poses(i)) {
       case mecab::Query::Noun:
         poses.insert("名詞");
         break;
       case mecab::Query::Verb:
         poses.insert("動詞");
         break;
       case mecab::Query::Adjective:
         poses.insert("形容詞");
         break;
      }
    }

    std::unique_ptr<MeCab::Model> model(MeCab::Model::create(""));
    std::unique_ptr<MeCab::Tagger> tagger(model->createTagger());

    size_t begin = 0;
    size_t num_tokens = 0;
    for (const MeCab::Node* node = tagger->parseToNode(request->document().c_str()); node; node = node->next) {
      if (node->stat == MECAB_BOS_NODE || node->stat == MECAB_EOS_NODE) continue;
      ++num_tokens;

      std::string feature = node->feature;
      std::string pos = feature.substr(0, feature.find(','));
      if (poses.find(pos) == poses.end()) continue;

      ::mecab::MeCabResult_Token* new_token = response->add_tokens();
      new_token->set_surface(std::string(node->surface, node->surface + node->length));
      new_token->set_begin(begin);
      new_token->set_len(node->length);
      begin += node->rlength;
    }
    response->set_num_all_tokens(num_tokens);
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
