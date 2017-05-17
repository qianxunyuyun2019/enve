#include "memoryhandler.h"
#include "Boxes/boundingboxrendercontainer.h"
MemoryHandler *MemoryHandler::mInstance;

MemoryHandler::MemoryHandler(QObject *parent) : QObject(parent) {
    mInstance = this;
    mMemoryChekerThread = new QThread(this);
    mMemoryChecker = new MemoryChecker();
    mMemoryChecker->moveToThread(mMemoryChekerThread);

    connect(mMemoryChecker, SIGNAL(freeMemory(unsigned long long)),
            this, SLOT(freeMemory(unsigned long long)) );

    mMemoryChekerThread->start();
}

MemoryHandler::~MemoryHandler() {
    mMemoryChekerThread->quit();
    mMemoryChekerThread->wait();
}

void MemoryHandler::addContainer(CacheContainer *cont) {
    mContainers << cont;
}

void MemoryHandler::removeContainer(CacheContainer *cont) {
    mContainers.removeOne(cont);
}

void MemoryHandler::containerUpdated(CacheContainer *cont) {
    removeContainer(cont);
    addContainer(cont);
}

void MemoryHandler::freeMemory(const unsigned long long &bytes) {
    long long memToFree = bytes;
    int unfreeable = 0;
    while(memToFree > 0 && mContainers.count() > unfreeable) {
        CacheContainer *cont = mContainers.takeFirst();
        int byteCount = cont->getByteCount();
        if(cont->freeThis()) {
            memToFree -= byteCount;
        } else {
            unfreeable++;
            mContainers << cont;
        }
    }
    if(memToFree > 0) {
        emit allMemoryUsed();
    }
}