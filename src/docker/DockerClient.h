
#pragma once
#include "RESTClient.h"



namespace Sphinx {
namespace Docker {


template <typename T = UnixSocket>
class DockerClient : protected RESTClient<T> {
    private:
        using RESTClient<T>::get;
    public:
        using RESTClient<T>::RESTClient;

        std::string getContainers()
        {
            return get("/containers/json?all=1");
        }
        std::string getInfo()
        {
            return get("/info");
        }



};

inline 
auto make_docker_client(const std::string &address, unsigned short port) {
    return DockerClient<TCPSocket>{address, port};
}
inline 
auto make_docker_client(const std::string &socket_path) {
    return DockerClient<UnixSocket>{socket_path};
}


} // namespace Docker
} // namespace Sphinx
