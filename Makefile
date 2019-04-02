CC=g++
CXX=g++
LD=g++

CXXFLAGS= -std=c++11
CPPFLAGS= $(shell pkg-config --cflags protobuf grpc) -I.
LDFLAGS=$(shell pkg-config --libs protobuf grpc++ grpc) \
  -lgrpc++_reflection -ldl

PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

.PHONY: all
all: mecab_server mecab_client

.PHONY: clean
clean:
	$(RM) $(wildcard *.pb.o) mecab_server.o
	$(RM) $(wildcard *.pb.cc) $(wildcard *.pb.h)

mecab_server: mecab.pb.o mecab.grpc.pb.o mecab_server.o
mecab_client: mecab.pb.o mecab.grpc.pb.o mecab_client.o

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) --cpp_out=. $<

# 自動生成のための依存関係追加
mecab_server.o: mecab.pb.cc mecab.grpc.pb.cc
mecab_client.o: mecab.pb.cc mecab.grpc.pb.cc
