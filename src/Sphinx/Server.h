
#pragma once

#include <atomic>
#include <Poco/Logger.h>

namespace Sphinx {
    class Server
    {
        public:
            Server ();
            ~Server ();
            const char * name() { return "Sphinx::Server"; }

            void listen();

        private:
            /* data */
            Poco::Logger& logger;
            std::atomic<bool> terminate;

    };

}
