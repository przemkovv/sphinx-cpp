
#pragma once
#include "RESTClient.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop



namespace Sphinx {
namespace Docker {

using json = nlohmann::json;

template <typename T = UnixSocket>
class DockerClient : protected RESTClient<T> {
    private:
        using RESTClient<T>::get;
    public:
        using RESTClient<T>::RESTClient;

        std::string getContainers()
        {
            auto data= get("/containers/json?all=1&size=1").second;
            auto j1 = json::parse(data);

            return j1.dump(4);
        }
        std::string getInfo()
        {
            auto data= get("/info").second;
            auto j1 = json::parse(data);

            return j1.dump(4);
        
        }



};

inline
auto make_docker_client(const std::string& address, unsigned short port)
{
    return DockerClient<TCPSocket> {address, port};
}
inline
auto make_docker_client(const std::string& socket_path)
{
    return DockerClient<UnixSocket> {socket_path};
}


} // namespace Docker
} // namespace Sphinx
