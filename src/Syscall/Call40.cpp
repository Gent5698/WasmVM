/*
Copyright (c) 2017 Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use
this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations
under the License.
*/
#include <Call40.h>

void Call::sysExit(Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 1) {
    throw Exception("[syscall][sys_exit] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* operand = (Value*)coreStack.pop().data;
  if (operand->type != i32) {
    throw Exception("[syscall][sys_exit] Operand type is not i32.", coreStack);
  }
  // Sys_exit
  exit(operand->data.i32);
  // Clean
  delete operand;
}
void Call::sysKill(Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_kill] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* sig = (Value*)coreStack.pop().data;
  if (sig->type != i32) {
    throw Exception("[syscall][sys_kill] signal type is not i32.", coreStack);
  }
  Value* pid = (Value*)coreStack.pop().data;
  if (pid->type != i32) {
    throw Exception("[syscall][sys_kill] pid type is not i32.", coreStack);
  }
  // Sys_kill
  std::int32_t ret = kill(pid->data.i32, sig->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_kill] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete sig;
  delete pid;
}
void Call::sysPause(Stack& coreStack) {
  // Sys_pause
  std::int32_t ret = pause();
  if (errno) {
    throw Exception(std::string("[syscall][sys_pause] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
}
void Call::sysGetpid(Stack& coreStack) {
  // Sys_getpid
  std::int32_t ret = getpid();
  coreStack.push(ret);
}
void Call::sysExecve(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 3) {
    throw Exception("[syscall][sys_execve] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* envAddr = (Value*)coreStack.pop().data;
  Value* argvAddr = (Value*)coreStack.pop().data;
  Value* fileNameAddr = (Value*)coreStack.pop().data;
  if (envAddr->type != i32 || argvAddr->type != i32 ||
      fileNameAddr->type != i32) {
    throw Exception("[syscall][sys_execve] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_execve] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_execve] Memory not exists in the store.",
                    coreStack);
  }
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* fileNamePtr = memoryData += fileNameAddr->data.i32;
  char* argvPtr = memoryData += argvAddr->data.i32;
  char* envPtr = memoryData += envAddr->data.i32;
  // Get argv
  std::vector<char*> argv;
  do {
    argv.push_back(argvPtr);
    argvPtr += strlen(argvPtr) + 1;
  } while (*(std::int32_t*)argvPtr);
  argv.push_back(NULL);
  // Get env
  std::vector<char*> env;
  do {
    env.push_back(envPtr);
    envPtr += strlen(envPtr) + 1;
  } while (*(std::int32_t*)envPtr);
  env.push_back(NULL);
  // Sys_execve
  std::int32_t ret =
      execve(fileNamePtr, (char**)argv.data(), (char**)env.data());
  if (errno) {
    throw Exception(std::string("[syscall][sys_execve] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete envAddr;
  delete argvAddr;
  delete fileNameAddr;
}
void Call::sysFork(Stack& coreStack) {
  // Sys_fork
  std::int32_t ret = fork();
  if (errno) {
    throw Exception(std::string("[syscall][sys_fork] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
}
void Call::sysVfork(Stack& coreStack) {
  // Sys_fork
  std::int32_t ret = vfork();
  if (errno) {
    throw Exception(std::string("[syscall][sys_vfork] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
}
void Call::sysSocket(Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_socket] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* sockProto = (Value*)coreStack.pop().data;
  Value* sockType = (Value*)coreStack.pop().data;
  Value* sockDomain = (Value*)coreStack.pop().data;
  if (sockProto->type != i32 || sockType->type != i32 ||
      sockDomain->type != i32) {
    throw Exception("[syscall][sys_socket] value types are not i32.",
                    coreStack);
  }
  // Sys_socket
  std::int32_t ret =
      socket(sockDomain->data.i32, sockType->data.i32, sockProto->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_socket] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete sockProto;
  delete sockType;
  delete sockDomain;
}
void Call::sysShutdown(Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_shutdown] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* how = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (sockfd->type != i32 || how->type != i32) {
    throw Exception("[syscall][sys_shutdown] value types are not i32.",
                    coreStack);
  }
  // Sys_shutdown
  std::int32_t ret = shutdown(sockfd->data.i32, how->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_shutdown] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete sockfd;
  delete how;
}
void Call::sysConnect(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 3) {
    throw Exception("[syscall][sys_connect] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* addrLen = (Value*)coreStack.pop().data;
  Value* sockAddr = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (sockfd->type != i32 || sockAddr->type != i32 || addrLen->type != i32) {
    throw Exception("[syscall][sys_connect] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_connect] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_connect] Memory not exists in the store.",
                    coreStack);
  }
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* addrPtr = memoryData += sockAddr->data.i32;
  // Sys_connect
  std::int32_t ret =
      connect(sockfd->data.i32, (struct sockaddr*)addrPtr, addrLen->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_connect] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete addrLen;
  delete sockAddr;
  delete sockfd;
}
void Call::sysBind(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 3) {
    throw Exception("[syscall][sys_bind] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* addrLen = (Value*)coreStack.pop().data;
  Value* sockAddr = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (sockfd->type != i32 || sockAddr->type != i32 || addrLen->type != i32) {
    throw Exception("[syscall][sys_bind] value types are not i32.", coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_bind] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_bind] Memory not exists in the store.",
                    coreStack);
  }
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* addrPtr = memoryData += sockAddr->data.i32;
  // Sys_bind
  std::int32_t ret =
      bind(sockfd->data.i32, (struct sockaddr*)addrPtr, addrLen->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_bind] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete addrLen;
  delete sockAddr;
  delete sockfd;
}
void Call::sysListen(Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_listen] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* backlog = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (sockfd->type != i32 || backlog->type != i32) {
    throw Exception("[syscall][sys_listen] value types are not i32.",
                    coreStack);
  }
  // Sys_listen
  std::int32_t ret = listen(sockfd->data.i32, backlog->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_listen] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete backlog;
  delete sockfd;
}
void Call::sysAccept(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 3) {
    throw Exception("[syscall][sys_accept] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* addrLen = (Value*)coreStack.pop().data;
  Value* sockAddr = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (sockfd->type != i32 || sockAddr->type != i32 || addrLen->type != i32) {
    throw Exception("[syscall][sys_accept] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_accept] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_accept] Memory not exists in the store.",
                    coreStack);
  }
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* addrPtr = memoryData += sockAddr->data.i32;
  char* addrLenPtr = memoryData += addrLen->data.i32;
  // Sys_accept
  std::int32_t ret = accept(sockfd->data.i32, (struct sockaddr*)addrPtr,
                            (socklen_t*)addrLenPtr);
  if (errno) {
    throw Exception(std::string("[syscall][sys_accept] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete addrLen;
  delete sockAddr;
  delete sockfd;
}
void Call::sysSendto(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 6) {
    throw Exception("[syscall][sys_sendto] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* addrLen = (Value*)coreStack.pop().data;
  Value* destAddr = (Value*)coreStack.pop().data;
  Value* flags = (Value*)coreStack.pop().data;
  Value* len = (Value*)coreStack.pop().data;
  Value* bufAddr = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (bufAddr->type != i32 || len->type != i32 || flags->type != i32 ||
      destAddr->type != i32 || addrLen->type != i32 || sockfd->type != i32) {
    throw Exception("[syscall][sys_sendto] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_sendto] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_sendto] Memory not exists in the store.",
                    coreStack);
  }
  // Get pointer
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* destPtr = memoryData += destAddr->data.i32;
  char* bufPtr = memoryData += bufAddr->data.i32;
  // Sys_sendto
  std::int32_t ret =
      sendto(sockfd->data.i32, bufPtr, len->data.i32, flags->data.i32,
             (struct sockaddr*)destPtr, addrLen->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_sendto] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete addrLen;
  delete destAddr;
  delete flags;
  delete len;
  delete bufAddr;
  delete sockfd;
}

void Call::sysRecvfrom(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 6) {
    throw Exception("[syscall][sys_recvfrom] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* addrLenAddr = (Value*)coreStack.pop().data;
  Value* srcAddr = (Value*)coreStack.pop().data;
  Value* flags = (Value*)coreStack.pop().data;
  Value* len = (Value*)coreStack.pop().data;
  Value* bufAddr = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (bufAddr->type != i32 || len->type != i32 || flags->type != i32 ||
      srcAddr->type != i32 || addrLenAddr->type != i32 || sockfd->type != i32) {
    throw Exception("[syscall][sys_recvfrom] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_recvfrom] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_recvfrom] Memory not exists in the store.",
                    coreStack);
  }
  // Get pointer
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* srcPtr = memoryData += srcAddr->data.i32;
  char* bufPtr = memoryData += bufAddr->data.i32;
  char* addrLenPtr = memoryData += addrLenAddr->data.i32;
  // Sys_recvfrom
  std::int32_t ret =
      recvfrom(sockfd->data.i32, bufPtr, len->data.i32, flags->data.i32,
               (struct sockaddr*)srcPtr, (unsigned int*)addrLenPtr);
  if (errno) {
    throw Exception(std::string("[syscall][sys_recvfrom] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete addrLenAddr;
  delete srcAddr;
  delete flags;
  delete len;
  delete bufAddr;
  delete sockfd;
}
void Call::sysDup2(Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_dup2] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* newfd = (Value*)coreStack.pop().data;
  Value* oldfd = (Value*)coreStack.pop().data;
  if (oldfd->type != i32 || newfd->type != i32) {
    throw Exception("[syscall][sys_dup2] value types are not i32.", coreStack);
  }
  // Sys_dup2
  std::int32_t ret = dup2(oldfd->data.i32, newfd->data.i32);
  if (errno) {
    throw Exception(std::string("[syscall][sys_dup2] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete oldfd;
  delete newfd;
}
void Call::sysNanosleep(Store& store, Stack& coreStack) {
  // Check value count
  if (coreStack.valueNum < 2) {
    throw Exception("[syscall][sys_nanosleep] No enough value in the stack.",
                    coreStack);
  }
  // Pop operand
  Value* addrLenAddr = (Value*)coreStack.pop().data;
  Value* srcAddr = (Value*)coreStack.pop().data;
  Value* flags = (Value*)coreStack.pop().data;
  Value* len = (Value*)coreStack.pop().data;
  Value* bufAddr = (Value*)coreStack.pop().data;
  Value* sockfd = (Value*)coreStack.pop().data;
  if (bufAddr->type != i32 || len->type != i32 || flags->type != i32 ||
      srcAddr->type != i32 || addrLenAddr->type != i32 || sockfd->type != i32) {
    throw Exception("[syscall][sys_nanosleep] value types are not i32.",
                    coreStack);
  }
  // Check memory address
  if (coreStack.curFrame->moduleInst->memaddrs.size() < 1) {
    throw Exception("[syscall][sys_nanosleep] No memory exists in this module.",
                    coreStack);
  }
  // Check memory
  std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
  if (memAddr >= store.mems.size()) {
    throw Exception("[syscall][sys_nanosleep] Memory not exists in the store.",
                    coreStack);
  }
  // Get pointer
  char* memoryData = store.mems.at(memAddr)->data.data();
  char* srcPtr = memoryData += srcAddr->data.i32;
  char* bufPtr = memoryData += bufAddr->data.i32;
  char* addrLenPtr = memoryData += addrLenAddr->data.i32;
  // Sys_recvfrom
  std::int32_t ret =
      recvfrom(sockfd->data.i32, bufPtr, len->data.i32, flags->data.i32,
               (struct sockaddr*)srcPtr, (unsigned int*)addrLenPtr);
  if (errno) {
    throw Exception(std::string("[syscall][sys_nanosleep] ") + strerror(errno),
                    coreStack);
  }
  coreStack.push(ret);
  // Clean
  delete addrLenAddr;
  delete srcAddr;
  delete flags;
  delete len;
  delete bufAddr;
  delete sockfd;
}
