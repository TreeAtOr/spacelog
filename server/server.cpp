#include <arpa/inet.h>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <hash_set>

using namespace std;
#include "../lib/protocol.h"


void log_local(const std::string message) {
  std::cout << "[" << getpid() << "] " << message << std::endl;
}

string getLevel(LOG_LEVEL level) {
  switch (level) {
  case info:
    return "info";
  case warn:
    return "warn";
  case error:
    return "error";
  default:
    return "unknow";
  }
}

sockaddr_in servAddr;
int serverSd;

void config(const int port) {
  sockaddr_in servAddr;
  bzero((char *)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(port);

  serverSd = socket(AF_INET, SOCK_STREAM, 0);

  if (serverSd < 0) {
    cerr << "Error establishing the server socket" << endl;
    exit(0);
  }

  int bindStatus =
      bind(serverSd, (struct sockaddr *)&servAddr, sizeof(servAddr));
  if (bindStatus < 0) {
    cerr << "Error binding socket to local address" << endl;
    exit(0);
  }
}

std::mutex socket_lock;
__gnu_cxx::hash_set<int> sockets;

void handler() {
  sockaddr_in newSockAddr;
  socklen_t newSockAddrSize = sizeof(newSockAddr);
  int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
  if (newSd < 0) {
      cerr << "Error accepting request from client!" << endl;
      return;
  }
  socket_lock.lock();
  sockets.insert(newSd);
  socket_lock.unlock();
}

ofstream output;
void log(const LoggingRequest &request) {
    output << "[" << request.cid  << " " << getLevel(request.log_level) << " ] " 
       << request.msg << endl;
}
void log_reader() {
    
  socket_lock.lock();
  Request request;
  for (auto socket: sockets) {
    memset(&request, 0, sizeof(Request));
    auto res = recv(socket, (char *)&request, sizeof(Request), 0);
    if (res == -1) {
        sockets.erase(socket);
    }

    if (res != 0) {
      if (request.isLogging) {
        log(request.logging);
      }
    }
  }

  output.flush();
  socket_lock.unlock();
}

void _handler_thread_f() {
    log_local(to_string(gettid()) + " _handler_thread_f");
    while (true) {
        handler();
    }
}

void _log_thread_f() {
    log_local(to_string(gettid()) + " _log_thread_f");
    while (true) {
        log_reader();
    }
}


int main(int argc, char *argv[]) {
  int port = atoi(getenv("PORT"));
  output.open(getenv("DB_FILE"), ios_base::out);
  if(!output.is_open()) {
    log_local("File not found");
  }

  config(port);

  log_local(to_string(gettid()) + " main");
  log_local("Waiting for a client to connect...");
  listen(serverSd, -1);

  thread handling_thread(&_handler_thread_f);
  thread logging_thread(&_log_thread_f);

  handling_thread.join();
  logging_thread.join();

  output.close();

  return 0;
}
