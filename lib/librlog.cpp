#include <arpa/inet.h>
#include <cstddef>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
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
#include <unistd.h>
#include <random>
#include "./protocol.h"

sockaddr_in sendSockAddr;
int clientSd;

std::random_device rd;  
std::mt19937 gen(rd());
int cid = (unsigned int)gen();

void log_local(const std::string message) {
    std::cout << "[" << getpid() << "] " << message << std::endl;
}

void config(const char* hostname, int port) {
    struct hostent *host = gethostbyname(hostname);
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
};

void connect() {
    clientSd = socket(AF_INET, SOCK_STREAM, 0);
    int status = connect(clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
    if (status < 0) {
        log_local("Error connecting to socket!");
        return;
    }
    log_local("Connected to the server!");
}

void sendRequest(Request *req) {
    send(clientSd, (void *)req, sizeof(Request), 0);
}

void log_remote(LOG_LEVEL level,const std::string &log) {
    log_local(log);
    Request logRequest;
    logRequest.isLogging = 1;
    logRequest.logging.log_level = level;
    logRequest.logging.cid = cid;
    if(log.length() > MAX) {
        log_remote(level,log.substr(0, MAX));
        log_remote(level,log.substr(MAX, log.length()));
        return;
    }
    auto charstr = log.c_str();
    auto dest = logRequest.logging.msg;
    strcpy(dest, charstr);
    sendRequest(&logRequest);
}

void sendSuccess(long double value) {
    Request successRequest;
    successRequest.isLogging = false;
    successRequest.success.result = value;
    sendRequest(&successRequest);
}