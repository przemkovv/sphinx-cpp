
#pragma once

#include "IOConnection.h"

#include "Logger.h"
#include <fmt/format.h>
#include "pprint.hpp"

#include <boost/asio.hpp>
#include <string>

#include <regex>

namespace Sphinx {
namespace Docker {

enum class http_status {
    ERROR = -1,		/* An error response from httpXxxx() */

    CONTINUE = 100,		/* Everything OK, keep going... */
    SWITCHING_PROTOCOLS,	/* HTTP upgrade to TLS/SSL */

    OK = 200,		/* OPTIONS/GET/HEAD/POST/TRACE command was successful */
    CREATED,			/* PUT command was successful */
    ACCEPTED,		/* DELETE command was successful */
    NOT_AUTHORITATIVE,	/* Information isn't authoritative */
    NO_CONTENT,		/* Successful command, no new data */
    RESET_CONTENT,		/* Content was reset/recreated */
    PARTIAL_CONTENT,		/* Only a partial file was recieved/sent */

    MULTIPLE_CHOICES = 300,	/* Multiple files match request */
    MOVED_PERMANENTLY,	/* Document has moved permanently */
    MOVED_TEMPORARILY,	/* Document has moved temporarily */
    SEE_OTHER,		/* See this other link... */
    NOT_MODIFIED,		/* File not modified */
    USE_PROXY,		/* Must use a proxy to access this URI */

    BAD_REQUEST = 400,	/* Bad request */
    UNAUTHORIZED,		/* Unauthorized to access host */
    PAYMENT_REQUIRED,	/* Payment required */
    FORBIDDEN,		/* Forbidden to access this URI */
    NOT_FOUND,		/* URI was not found */
    METHOD_NOT_ALLOWED,	/* Method is not allowed */
    NOT_ACCEPTABLE,		/* Not Acceptable */
    PROXY_AUTHENTICATION,	/* Proxy Authentication is Required */
    REQUEST_TIMEOUT,		/* Request timed out */
    CONFLICT,		/* Request is self-conflicting */
    GONE,			/* Server has gone away */
    LENGTH_REQUIRED,		/* A content length or encoding is required */
    PRECONDITION,		/* Precondition failed */
    REQUEST_TOO_LARGE,	/* Request entity too large */
    URI_TOO_LONG,		/* URI too long */
    UNSUPPORTED_MEDIATYPE,	/* The requested media type is unsupported */
    UPGRADE_REQUIRED = 426,	/* Upgrade to SSL/TLS required */

    SERVER_ERROR = 500,	/* Internal server error */
    NOT_IMPLEMENTED,		/* Feature not implemented */
    BAD_GATEWAY,		/* Bad gateway */
    SERVICE_UNAVAILABLE,	/* Service is unavailable */
    GATEWAY_TIMEOUT,		/* Gateway connection timed out */
    NOT_SUPPORTED		/* HTTP version not supported */
} ;

enum class HTTPMethod {
    POST,
    GET
};

using HTTPHeaders = std::map<std::string, std::string>;
struct HTTPResponse;

class HTTPRequest {
        HTTPMethod method;
        std::string uri;
        HTTPHeaders headers;
        std::string data;

        void addDefaultHeaders()
        {
            addHeader("Host", "localhost");
            addHeader("Accept", "*/*");
            addHeader("Connection", "close");
        }

    protected:
        HTTPRequest() {}
    public:
        HTTPRequest(HTTPMethod method, const std::string& uri) :  method(method), uri(uri)
        {
            addDefaultHeaders();
        }
        HTTPRequest(HTTPMethod method, const std::string& uri, const std::string& data)
            : method(method), uri(uri), data(data)
        {
            addDefaultHeaders();
            addHeader("Content-Length", data.size());
            addHeader("Content-Type", "application/json");
        }

        template <typename T>
        void addHeader(const std::string& name, const T& value)
        {
            headers.insert(std::make_pair(name, fmt::format("{}", value)));
        }

        std::string to_string() const
        {
            fmt::MemoryWriter w;

            switch (method) {
                case HTTPMethod::GET:
                    w.write("GET");
                    break;

                case HTTPMethod::POST:
                    w.write("POST");
                    break;
            }

            w.write(" {0} HTTP/1.1\r\n", uri);

            for (auto header : headers) {
                w.write("{0}: {1}\r\n", header.first, header.second);
            }

            w.write("\r\n");

            if (!data.empty()) {
                w.write("{0}", data);
            }

            return w.str();
        }

        friend struct HTTPResponse;

};

struct HTTPResponse {
    http_status status;
    HTTPHeaders headers;
    std::string data;

    HTTPRequest request;

    HTTPResponse(std::string headers, std::string data):headers(parse_headers(headers)), data(data)
    {
        std::regex status_regex("^HTTP/\\d\\.\\d (\\d{3}) .+");
        std::smatch sm;

        if (std::regex_search(headers, sm, status_regex)) {
            status = static_cast<http_status>(std::stoi(sm[1]));
        }
    }

    HTTPHeaders parse_headers(std::string headers_str)
    {
        HTTPHeaders headers;
        std::regex header_regex("(.+): (.+)\r\n");
        std::smatch sm;

        while (std::regex_search(headers_str, sm, header_regex)) {
            headers.insert({sm[1], sm[2]});
            headers_str = sm.suffix();
        }

        return headers;
    }

    std::string to_string() const
    {
        fmt::MemoryWriter w;
        w.write("Request: {0}\n", request.to_string());
        w.write("Response status: {0}\n", static_cast<int>(status));
        w.write("Response headers: \n");

        for (auto header : headers) {
            w.write("{0}: {1}\r\n", header.first, header.second);
        }

        w.write("Response data: {0}\n", data);
        return  w.str();
    }
};

template <typename T = TCPSocket>
class RESTClient : protected IOConnection<T> {
        using IOConnection<T>::send;
        using IOConnection<T>::receive;
        using IOConnection<T>::reconnect;

    public:
        using IOConnection<T>::IOConnection;

        auto receive_response()
        {
            auto response = receive();
            auto header_end = response.find("\r\n\r\n");
            auto header = response.substr(0, header_end+2);
            auto response_data = response.substr(header_end+4);

            if (is_chunked(header)) {
                response_data = extract_chunk_data(response_data);
            }

            auto http_response = HTTPResponse {header, response_data};
            logger->debug("HTTP Status code: {0}", static_cast<int>(http_response.status));
            logger->debug("HTTP Response headers:\n{}", http_response.headers);
            logger->debug("HTTP Response:\n{}", http_response.data);
            return http_response;
        }

        auto post(const std::string& path)
        {
            reconnect();
            auto  request = HTTPRequest {HTTPMethod::POST, path};
            logger->debug("HTTP Request:\n{}", request.to_string());
            send(request.to_string());
            auto response = receive_response();
            response.request = request;
            return response;
        }
        auto post(const std::string& path, const std::string& data)
        {
            reconnect();
            auto  request = HTTPRequest {HTTPMethod::POST, path, data};
            logger->debug("HTTP Request:\n{}", request.to_string());
            send(request.to_string());
            auto response = receive_response();
            response.request = request;
            return response;
        }

        auto get(const std::string& path)
        {
            reconnect();
            auto  request = HTTPRequest {HTTPMethod::GET, path};
            logger->debug("HTTP Request:\n{}", request.to_string());
            send(request.to_string());
            auto response = receive_response();
            response.request = request;
            return response;
        }


    private:
        bool is_chunked(const std::string& headers)
        {
            const auto chunked_header = "Transfer-Encoding: chunked";
            return headers.find(chunked_header) != std::string::npos;
        }

        std::string extract_chunk_data(std::string data)
        {
            auto chunk_size_end_position = data.find("\r\n");
            auto chunk_size_hex = data.substr(0, chunk_size_end_position);
            auto chunk_size = static_cast<size_t>(stoi(chunk_size_hex, 0, 16));
            logger->debug("Chunk size: {} (0x{}), position: {}", chunk_size, chunk_size_hex, chunk_size_end_position);
            data.erase(0, chunk_size_end_position+2);
            return data.substr(0, chunk_size);
        }

        Logger logger = make_logger("HTTPClient");
};
} // namespace Docker
} // namespace Sphinx
