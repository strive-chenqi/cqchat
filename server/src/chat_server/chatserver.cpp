// #include "logicsystem.h"
#include <csignal>
#include <mutex>
#include "asioservicepool.h"
#include "connectionserver.h"
#include "configmgr.h"
using namespace std;
bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main()
{
    try {
        auto &cfg = ConfigMgr::Inst();
        auto pool = AsioIOServicePool::GetInstance();
        boost::asio::io_context  io_context;
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context, pool](auto, auto) {
            io_context.stop();
            pool->stop();
            });
        auto port_str = cfg["SelfServer"]["Port"];
        unsigned short port = static_cast<unsigned short>(atoi(port_str.c_str()));
        CServer s(io_context, port);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << endl;
    }

}