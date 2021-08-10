#include "TcpAcceptor.h"
#include "SocketOperation.h"
#include <iostream>
#include <unistd.h>

using namespace yy;
using namespace std;

TcpAcceptor::TcpAcceptor(EventLoop *loop, SocketAddr &addr) :
    loop_(loop),
    socket_(new Socket(SocketOperation::createNonblockingSocket())),
    event_(new Channel(loop,socket_->getFd())),
    listening_(false)
{
  socket_->setReusePort(true);
  socket_->bind(addr);
  event_->setReadCallback(std::bind(&TcpAcceptor::acceptHandle,this));
  loop_->updateChannel(event_.get());
}

TcpAcceptor::~TcpAcceptor() {
  event_->disableAll();
//  event_->removeFromLoop();//TODO
}

void TcpAcceptor::listen() {
  listening_=true;
  socket_->listen();
  event_->enableReading();
  cout<<"Server is listening\n";
}

bool TcpAcceptor::isListen() {
  return listening_;
}

void TcpAcceptor::setNewConnectCallback(const NewConnectCallback &callback) {
  newConnectCallback_=callback;
}

void TcpAcceptor::acceptHandle() {
  SocketAddr connectAddr;
  int connectfd=-1;
  if((connectfd=socket_->accept(connectAddr))>0){
    if(newConnectCallback_){
      newConnectCallback_(connectfd,connectAddr);
    }else{
      ::close(connectfd);
    }
  }
}