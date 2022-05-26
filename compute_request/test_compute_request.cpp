#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/compute_request_ui.h"
#include "ports.h"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace compute_request;

int main() {
  std::shared_ptr<apache::thrift::transport::TTransport> socket(
      new apache::thrift::transport::TSocket("localhost", UI_PORT));
  std::shared_ptr<apache::thrift::transport::TTransport> transport(
      new apache::thrift::transport::TBufferedTransport(socket));
  std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
      new apache::thrift::protocol::TBinaryProtocol(transport));
  compute_request_uiClient ui(protocol);

  transport->open();

  ui.create_provider("AliceA", "password", 2);
  ui.post_compute("AliceA", "password", 30);

  ui.create_provider("BobbyB", "password", 4);
  ui.post_compute("BobbyB", "password", 5);

  ui.create_provider("CarolC", "password", 6);
  ui.post_compute("CarolC", "password", 10);


  ui.create_client("DaveyD", "password", 1);
  Compute _return;
  ui.get_compute(_return, "DaveyD", "password", 10);
  printf("DaveyD's result, this should fail because he has a low level\n");
  std::cout << _return.id << " " << _return.level << " " << _return.price << " " << _return.time << " " << _return.valid << "\n";

  ui.create_client("EllenE", "password", 5);
  ui.get_compute(_return, "EllenE", "password", 10);
  printf("EllenE's first result, this should get Alice's resource\n");
  std::cout << _return.id << " " << _return.level << " " << _return.price << " " << _return.time << " " << _return.valid << "\n";

  ui.get_compute(_return, "EllenE", "password", 10);
  printf("EllenE's second result, this should fail\n");
  std::cout << _return.id << " " << _return.level << " " << _return.price << " " << _return.time << " " << _return.valid << "\n";

  ui.create_client("FrancesF", "password", 9);
  ui.get_compute(_return, "FrancesF", "password", 10);
  printf("FrancesF's first result, this should get Carol's resource\n");
  std::cout << _return.id << " " << _return.level << " " << _return.price << " " << _return.time << " " << _return.valid << "\n";

  ui.get_compute(_return, "FrancesF", "password", 1);
  printf("FrancesF's second result, this should get Bobby's resource\n");
  std::cout << _return.id << " " << _return.level << " " << _return.price << " " << _return.time << " " << _return.valid << "\n";

  ui.get_compute(_return, "FrancesF", "password", 1);
  printf("FrancesF's third result, this should fail\n");
  std::cout << _return.id << " " << _return.level << " " << _return.price << " " << _return.time << " " << _return.valid << "\n";

  transport->close();
}