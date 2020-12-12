#include "GbProcessManager.h"
#include "GbThreadedProcess.h"
#include "GbLoadProcess.h"

#include "../GbCoreEngine.h"
#include "../events/GbEventManager.h"
#include "../scene/GbScenario.h"

#include <QApplication>

#define USE_MULTITHREADING

namespace Gb {
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// ProcessManager
/////////////////////////////////////////////////////////////////////////////////////////////
ProcessManager::ProcessManager(CoreEngine* engine, size_t initialThreadCount):
    Manager(engine, "ProcessManager"),
    ProcessQueue(),
    m_threadPool(new QThreadPool())
{
    // Initialize threads
    initializeThreads(initialThreadCount);
}
/////////////////////////////////////////////////////////////////////////////////////////////
ProcessManager::~ProcessManager()
{
    // Delete all sorting layers
    for (SortingLayer* layer : m_sortingLayers) {
        delete layer;
    }

    // Delete all dedicated process threads
    //for (ProcessThread* pt : m_dedicatedThreads) {
    //    delete pt;
    //}

    // Delete thread pool
    delete m_threadPool;
}
/////////////////////////////////////////////////////////////////////////////////////////////
WeakProcessPtr ProcessManager::attachProcess(StrongProcessPtr process, bool initialize)
{
    // Handle threaded vs unthreaded processes
    if (!process->isThreaded()) {
        // Add unthreaded process to process list
        ProcessQueue::attachProcess(process, initialize);
    }
    else {
        m_threadedProcessMutex.lock();

        // Add threaded process to thread pool queue
        auto threadedProcess = std::dynamic_pointer_cast<ThreadedProcess>(process);
        Vec::EmplaceBack(m_threadedProcesses, threadedProcess);
        m_threadedProcessMutex.unlock();

        //int count = m_threadPool->activeThreadCount();
        //int maxCount = m_threadPool->maxThreadCount();
#ifndef USE_MULTITHREADING
        threadedProcess->run();
#else
        auto future = QtConcurrent::run(m_threadPool, std::bind(&ThreadedProcess::run, threadedProcess));
#endif
        //future.waitForFinished();

        //m_threadPool->start(threadedProcess.get());
    }
    return WeakProcessPtr(process);
}
/////////////////////////////////////////////////////////////////////////////////////////////
WeakProcessPtr ProcessManager::reattachProcess(StrongProcessPtr process)
{
    // UNTESTED
    // Return if threaded process
    if (process->isThreaded()) {
        throw("Error, cannot safely reattach threaded process");
        return process;
    }

    // Find process in process set
    ProcessVec::const_iterator iter =
        std::find_if(m_processes.begin(), m_processes.end(),
            [&](const StrongProcessPtr& process) {
        return process->getUuid() == process->getUuid();
    });
    if (iter == m_processes.end()) {
        throw("Error, cannot reattach process that was never attached");
    }
    else {
        m_processes.erase(iter);
        m_processQueue.emplace_back(process);
    }

    return process;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::abortAllProcesses(bool immediate)
{
    // Clear standard processes
    ProcessQueue::abortAllProcesses(immediate);

    // Clear threaded processes
    if (m_threadedProcesses.size()) {
        QMutexLocker locker(&m_threadedProcessMutex);

        std::vector<StrongProcessPtr> threadedProcesses = m_threadedProcesses;
        std::vector<StrongProcessPtr>::iterator thrit = threadedProcesses.begin();
        while (thrit != threadedProcesses.end())
        {
            abortProcess(*thrit, immediate);
            ++thrit;
        }
    }

    // Clear processes on dedicated threads
    for (ProcessThread& thread : m_dedicatedThreads) {
        if (!thread.isRunning()) {
            continue;
        }
        thread.m_processMutex.lock();
        thread.abortAllProcesses(true);
        thread.m_processMutex.unlock();
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::postConstruction()
{
    // Initialize signal/slot connections
    deferredSenderConnect("EventManager",
        SIGNAL(deleteThreadedProcess(const Uuid&)),
        SLOT(deleteThreadedProcess(const Uuid&)));
}
/////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue ProcessManager::asJson() const
{
    QJsonObject json;

    // Add sorting layers to JSON
    QJsonArray sortingLayers;
    for (const SortingLayer* layer : m_sortingLayers) {
        if (layer->getName() != "default") {
            // Don't save default layer
            sortingLayers.append(layer->asJson());
        }
    }
    json.insert("sortingLayers", sortingLayers);

    return json;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::loadFromJson(const QJsonValue& json, const SerializationContext& context)
{
    Q_UNUSED(context);
    const QJsonObject& object = json.toObject();
    if (object.contains("sortingLayers")) {
        const QJsonArray& sortingLayers = object.value("sortingLayers").toArray();
        for (const auto& layerJson : sortingLayers) {
            SortingLayer* newLayer = new SortingLayer(layerJson);
            m_sortingLayers.emplace_back(newLayer);
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::initializeThreads(unsigned int threadCount)
{
    // Set new threads to not expire
    m_threadPool->setExpiryTimeout(-1);

    // Create threads in thread pool
    for (size_t i = 0; i < threadCount; i++) {
        m_threadPool->reserveThread();
    }

#ifdef DEBUG_MODE
    int maxAllowableThreads = m_threadPool->maxThreadCount();
    int currentCount = m_threadPool->activeThreadCount();
    if (currentCount > maxAllowableThreads) {
        logWarning("Warning, thread count exceeds optimum number of threads");
    }
#endif

    THREAD_COUNT += threadCount;
    m_engine->THREAD_COUNT += threadCount;

    // Initialize dedicate threads
    // TODO: Create threads for things other than animations
    m_dedicatedThreads[(int)DedicatedThreadType::kAnimation].initialize();
}
/////////////////////////////////////////////////////////////////////////////////////////////
unsigned int ProcessManager::loadProcessCount()
{
    QMutexLocker locker(&m_threadedProcessMutex);

    int count = 0;
    for (StrongProcessPtr ptr : m_threadedProcesses) {
        if (QString(ptr->className()).contains("LoadProcess")) {
            count++;
        }
    }

    return count;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::clearAllProcesses()
{
    // Abort all processes
    abortAllProcesses(true);

    // Clear process queues
    QMutexLocker locker(&m_threadedProcessMutex);
    m_processes.clear();
    m_threadedProcesses.clear();
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::deleteThreadedProcess(const Uuid& uuid){

    QMutexLocker locker(&m_threadedProcessMutex);

    auto iter = std::find_if(m_threadedProcesses.begin(), m_threadedProcesses.end(),
        [&](const std::shared_ptr<Process>& process) {
        if (!process) { 
            return true;
        }
        else {
            return process->getUuid() == uuid;
        }
    });
    if (iter != m_threadedProcesses.end()) {
        m_threadedProcesses.erase(iter);
    }
    else {
#ifdef DEBUG_MODE
        logWarning("Warning, did not find threaded process to delete");
#endif
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::logMessage(const GString & message, LogLevel logLevel)
{
    QApplication::postEvent(m_engine,
        new LogEvent(namespaceName(), message, Process::GetThreadID(), logLevel));
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ProcessManager::abortProcess(StrongProcessPtr process, bool immediate)
{
    if (process->isAlive())
    {
        process->setState(Process::kAborted);
        if (immediate)
        {
            process->onAbort();

            // Remove from process set if not a threaded process
            ProcessVec::const_iterator iter = std::find_if(m_processes.begin(),
                m_processes.end(),
                [&](StrongProcessPtr p) {
                return p->getUuid() == process->getUuid();
            });
            if (iter != m_processes.end()) {
                m_processes.erase(iter);
                return;
            }

            // Remove from threaded process list if threaded
            QMutexLocker locker(&m_threadedProcessMutex);
            std::vector<StrongProcessPtr>::const_iterator titer = std::find_if(m_threadedProcesses.begin(),
                m_threadedProcesses.end(),
                [&](StrongProcessPtr p) {
                return p->getUuid() == process->getUuid();
            });
            if (titer != m_threadedProcesses.end()) {
                m_threadedProcesses.erase(titer);
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
unsigned int ProcessManager::THREAD_COUNT = 0;




/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces