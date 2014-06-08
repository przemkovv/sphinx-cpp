
#pragma once
#include <Poco/Logger.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>



class RootHandler : public Poco::Net::HTTPRequestHandler {
    public:
        void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
        {
            auto& logger = Poco::Logger::get("RootHandler");
            logger.information("Request from " + request.clientAddress().toString());
            response.setChunkedTransferEncoding(true);
            response.setContentType("text/html");
            auto& ostr = response.send();
            ostr << "Hello World";
        }

    private:
        /* data */
};
