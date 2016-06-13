
#pragma once
#include <Poco/Logger.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace Sphinx {

class RootHandler : public Poco::Net::HTTPRequestHandler {
    public:
        void handleRequest(Poco::Net::HTTPServerRequest& /*request*/, Poco::Net::HTTPServerResponse& response)
        {
            response.setChunkedTransferEncoding(true);
            response.setContentType("text/html");
            auto& ostr = response.send();
            ostr << "Hello World";
        }

    private:
        /* data */
};
} // namespace Sphinx
