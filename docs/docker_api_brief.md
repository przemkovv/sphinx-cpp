Endpoints
=========

| Containers                                                           |                                        |
|----------------------------------------------------------------------|----------------------------------------|
| List containers                                                      | GET /containers/json                   |
| Create a container                                                   | POST /containers/create                |
| Inspect a container                                                  | GET /containers/(id or name)/json      |
| List processes running inside a container                            | GET /containers/(id or name)/top       |
| Get container log                                                    | GET /containers/(id or name)/logs      |
| Inspect changes on a container’s filesystem                          | GET /containers/(id or name)/changes   |
| Export a container                                                   | GET /containers/(id or name)/export    |
| Get container stats based on resource usage                          | GET /containers/(id or name)/stats     |
| Resize a container TTY                                               | POST /containers/(id or name)/resize   |
| Start a container                                                    | POST /containers/(id or name)/start    |
| Stop a container                                                     | POST /containers/(id or name)/stop     |
| Restart a container                                                  | POST /containers/(id or name)/restart  |
| Kill a container                                                     | POST /containers/(id or name)/kill     |
| Update a container                                                   | POST /containers/(id or name)/update   |
| Rename a container                                                   | POST /containers/(id or name)/rename   |
| Pause a container                                                    | POST /containers/(id or name)/pause    |
| Unpause a container                                                  | POST /containers/(id or name)/unpause  |
| Attach to a container                                                | POST /containers/(id or name)/attach   |
| Attach to a container (websocket)                                    | GET /containers/(id or name)/attach/ws |
| Wait a container                                                     | POST /containers/(id or name)/wait     |
| Remove a container                                                   | DELETE /containers/(id or name)        |
| Retrieving information about files and folders in a container        | HEAD /containers/(id or name)/archive  |
| Get an archive of a filesystem resource in a container               | GET /containers/(id or name)/archive   |
| Extract an archive of files or folders to a directory in a container | PUT /containers/(id or name)/archive   |

Images
------

| Images                         |                            |
|--------------------------------|----------------------------|
| List Images                    | GET /images/json           |
| Build image from a Dockerfile  | POST /build                |
| Create an image                | POST /images/create        |
| Inspect an image               | GET /images/(name)/json    |
| Get the history of an image    | GET /images/(name)/history |
| Push an image on the registry  | POST /images/(name)/push   |
| Tag an image into a repository | POST /images/(name)/tag    |
| Remove an image                | DELETE /images/(name)      |
| Search images                  | GET /images/search         |

Misc
----

| Misc                                                     |                                    |
|----------------------------------------------------------|------------------------------------|
| Check auth configuration                                 | POST /auth                         |
| Display system-wide information                          | GET /info                          |
| Show the docker version information                      | GET /version                       |
| Ping the docker server                                   | GET /\_ping                        |
| Create a new image from a container’s changes            | POST /commit                       |
| Monitor Docker’s events                                  | GET /events                        |
| Get a tarball containing all images in a repository      | GET /images/(name)/get             |
| Get a tarball containing all images                      | GET /images/get                    |
| Load a tarball with a set of images and tags into docker | POST /images/load                  |
| Exec Create                                              | POST /containers/(id or name)/exec |
| Exec Start                                               | POST /exec/(id)/start              |
| Exec Resize                                              | POST /exec/(id)/resize             |
| Exec Inspect                                             | GET /exec/(id)/json                |

Volumes
-------

| Volumes          |                        |
|------------------|------------------------|
| List volumes     | GET /volumes           |
| Create a volume  | POST /volumes/create   |
| Inspect a volume | GET /volumes/(name)    |
| Remove a volume  | DELETE /volumes/(name) |

Networks
--------

| Networks                              |                                |
|---------------------------------------|--------------------------------|
| List networks                         | GET /networks                  |
| Inspect network                       | GET /networks/<network-id>     |
| Create a network                      | POST /networks/create          |
| Connect a container to a network      | POST /networks/(id)/connect    |
| Disconnect a container from a network | POST /networks/(id)/disconnect |
| Remove a network                      | DELETE /networks/(id)          |
