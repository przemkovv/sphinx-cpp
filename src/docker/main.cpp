
#include "Logger.h"
#include <boost/asio.hpp>

int main()
{
    auto logger = Sphinx::make_logger("docker");
    logger->info("Hello");
    boost::system::error_code ec;
    using namespace boost::asio;
    // what we need
    io_service svc;
    ip::tcp::socket sock(svc);
    sock.connect({ {}, 2375 }); // http://localhost:8087 for testing
    // send request
    std::string request("GET /containers/json?all=1 HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "\r\n\r\n");
    logger->info("Request:\n{}", request);
    sock.send(buffer(request));
    // read response
    std::string response;

    do {
        char buf[1024];
        size_t bytes_transferred = sock.receive(buffer(buf), {}, ec);

        if (!ec) { response.append(buf, buf + bytes_transferred); }
    } while (!ec);

    // print and exit
    logger->info("Response received:\n{} ", response);
    return 0;
}
