
#pragma once
#include "RESTClient.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "json.hpp"

#pragma clang diagnostic pop

#include <stdexcept>


namespace Sphinx {
namespace Docker {
namespace v1 {

using json = nlohmann::json;

struct Container {
    std::string id;
};



class cannot_create_container_exception : public std::runtime_error {
    public:
        cannot_create_container_exception(const HTTPResponse& response)
            : runtime_error(fmt::format("Could not create container.\n{0}\n", response.to_string()))
        {
        }
};
class container_doesnt_exist_exception : public std::runtime_error {
    public:
        container_doesnt_exist_exception(const Container& container)
            :runtime_error(fmt::format("Container with id: {0} doesn't exist", container.id))
        {
        }
};

class server_error : public std::runtime_error {
    public:
        server_error(const HTTPResponse& response)
            : runtime_error(fmt::format("Server error.\n{0}", response.to_string()))
        {
        }
};

class undefined_error : public std::runtime_error {
    public:
        undefined_error(const HTTPResponse& response)
            : runtime_error(fmt::format("Undefined error. \n{0}\n", response.to_string()))
        {}

};


template <typename T = UnixSocket>
class DockerClient : protected RESTClient<T> {
    private:
        using RESTClient<T>::get;
        using RESTClient<T>::post;

        Logger logger = make_logger("DockerClient");
    public:
        using RESTClient<T>::RESTClient;

        const std::string image_name = "sphinx-docker";

        std::string getImages()
        {
            auto data= get("/images/json?all=1").data;
            auto images = json::parse(data);
            return images.dump(4);
        }
        std::string getContainers()
        {
            auto data= get("/containers/json?all=1&size=1").data;
            auto containers = json::parse(data);
            return containers.dump(4);
        }
        std::string getInfo()
        {
            auto data= get("/info").data;
            auto j1 = json::parse(data);
            return j1.dump(4);
        }

        auto createContainer(const std::string& image_name, const std::vector<std::string>& commands)
        {
            json container = {
                {"Hostname",""},
                {"User",""},
                {"Memory",0},
                {"MemorySwap",0},
                {"AttachStdin",false},
                {"AttachStdout",true},
                {"AttachStderr",true},
                {"Tty",false},
                {"OpenStdin",false},
                {"StdinOnce",false},
                {"Cmd", commands},
                {"Image",image_name},
                {"WorkingDir","",}
            };
            const auto request_data = container.dump();
            auto response = post("/containers/create", request_data);

            if (response.status == http_status::CREATED) {
                auto data_json = json::parse(response.data);
                Container container { data_json["Id"] };
                return container;
            } else {
                throw cannot_create_container_exception {response};
            }
        }


        bool startContainer(const Container& container)
        {
            auto request = fmt::format("/containers/{0}/start", container.id);
            auto response = post(request);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"

            switch (response.status) {
                case http_status::NOT_FOUND: {
                    throw container_doesnt_exist_exception {container};
                }

                case http_status::SERVER_ERROR: {
                    throw server_error {response};
                }

                case http_status::NOT_MODIFIED:
                case http_status::NO_CONTENT: {
                    return true;
                }

                default:
                    throw undefined_error {response};
            }

#pragma clang diagnostic push
        }

        void attachContainer(const Container& container) 
        {
            auto request = fmt::format("/containers/{0}/attach?stream=0&logs=1&stdout=1&stderr=1", container.id);
            auto response = post(request, {{"Upgrade", "tcp"}});

            logger->info("Attach container: {0}", response.to_string());

        }


        void run()
        {
            //auto container = createContainer(image_name, {"/bin/zsh", "-c", "'I=1 && repeat 100  echo $((I++)) && sleep 1'"});

            auto container = createContainer(image_name, {"date"});
            startContainer(container);
            attachContainer(container);
            //1. Create the container
            //2. If the status code is 404, it means the image doesn’t exist:
            //    a. Try to pull it.
            //    b. Then, retry to create the container.
            //3. Start the container.
            //4. If you are not in detached mode:
            //5. Attach to the container, using logs=1 (to have stdout and stderr from the container’s start) and stream=1
            //6. If in detached mode or only stdin is attached, display the container’s id.
        }



};

inline
DockerClient<TCPSocket> make_docker_client(const std::string& address, unsigned short port)
{
    return DockerClient<TCPSocket>(address, port);
}
inline
DockerClient<UnixSocket> make_docker_client(const std::string& socket_path)
{
    return DockerClient<UnixSocket>(socket_path);
}


} // namespace v1
} // namespace Docker
} // namespace Sphinx
