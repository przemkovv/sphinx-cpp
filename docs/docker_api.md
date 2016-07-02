Endpoints
=========

Containers
--------------

# List containers

List containers

 GET /containers/json

Query parameters:

    all – 1/True/true or 0/False/false, Show all containers. Only running containers are shown by default (i.e., this defaults to false)
    limit – Show limit last created containers, include non-running ones.
    since – Show only containers created since Id, include non-running ones.
    before – Show only containers created before Id, include non-running ones.
    size – 1/True/true or 0/False/false, Show the containers sizes
    filters - a JSON encoded value of the filters (a map[string][]string) to process on the containers list. Available filters:
        exited=<int>; -- containers with exit code of <int> ;
        status=(created|restarting|running|paused|exited|dead)
        label=key or label="key=value" of a container label
        isolation=(default|process|hyperv) (Windows daemon only)
        ancestor=(<image-name>[:<tag>], <image id> or <image@digest>)
        before=(<container id> or <container name>)
        since=(<container id> or <container name>)
        volume=(<volume name> or <mount point destination>)
        network=(<network id> or <network name>)

Status codes:

    200 – no error
    400 – bad parameter
    500 – server error

# Create a container

Create a container

 POST /containers/create

Query parameters:

    name – Assign the specified name to the container. Must match /?[a-zA-Z0-9_-]+.

Status codes:
    201 – no error
    400 – bad parameter
    404 – no such container
    406 – impossible to attach (container not running)
    500 – server error

# Inspect a container

 GET /containers/(id or name)/json

Return low-level information on the container id

Query parameters:

    size – 1/True/true or 0/False/false, return container size information. Default is false.

Status codes:

    200 – no error
    404 – no such container
    500 – server error

# List processes running inside a container

 GET /containers/(id or name)/top

List processes running inside the container id. On Unix systems this is done by running the ps command. This endpoint is not supported on Windows.

Query parameters:

    ps_args – ps arguments to use (e.g., aux), defaults to -ef

Status codes:

    200 – no error
    404 – no such container
    500 – server error

# Get container logs

 GET /containers/(id or name)/logs

Get stdout and stderr logs from the container id

    Note: This endpoint works only for containers with the json-file or journald logging drivers.

Query parameters:

    details - 1/True/true or 0/False/flase, Show extra details provided to logs. Default false.
    follow – 1/True/true or 0/False/false, return stream. Default false.
    stdout – 1/True/true or 0/False/false, show stdout log. Default false.
    stderr – 1/True/true or 0/False/false, show stderr log. Default false.
    since – UNIX timestamp (integer) to filter logs. Specifying a timestamp will only output log-entries since that timestamp. Default: 0 (unfiltered)
    timestamps – 1/True/true or 0/False/false, print timestamps for every log line. Default false.
    tail – Output specified number of lines at the end of logs: all or <number>. Default all.

Status codes:

    101 – no error, hints proxy about hijacking
    200 – no error, no upgrade header found
    404 – no such container
    500 – server error

# Inspect changes on a container’s filesystem

 GET /containers/(id or name)/changes

Inspect changes on container id’s filesystem

Status codes:

    200 – no error
    404 – no such container
    500 – server error

# Export a container

 GET /containers/(id or name)/export

Export the contents of container id

Status codes:

    200 – no error
    404 – no such container
    500 – server error

# Get container stats based on resource usage

 GET /containers/(id or name)/stats

This endpoint returns a live stream of a container’s resource usage statistics.

The precpu_stats is the cpu statistic of last read, which is used for calculating the cpu usage percent. It is not the exact copy of the “cpu_stats” field.

Query parameters:

    stream – 1/True/true or 0/False/false, pull stats once then disconnect. Default true.

Status codes:

    200 – no error
    404 – no such container
    500 – server error

# Resize a container TTY

 POST /containers/(id or name)/resize

Resize the TTY for container with id. The unit is number of characters. You must restart the container for the resize to take effect.

Query parameters:

    h – height of tty session
    w – width

Status codes:

    200 – no error
    404 – No such container
    500 – Cannot resize container

# Start a container

 POST /containers/(id or name)/start

Start the container id

Query parameters:

    detachKeys – Override the key sequence for detaching a container. Format is a single character [a-Z] or ctrl-<value> where <value> is one of: a-z, @, ^, [, , or _.

Status codes:

    204 – no error
    304 – container already started
    404 – no such container
    500 – server error

# Stop a container

 POST /containers/(id or name)/stop

Stop the container id

Query parameters:

    t – number of seconds to wait before killing the container

Status codes:

    204 – no error
    304 – container already stopped
    404 – no such container
    500 – server error

# Restart a container

 POST /containers/(id or name)/restart

Restart the container id

Query parameters:

    t – number of seconds to wait before killing the container

Status codes:

    204 – no error
    404 – no such container
    500 – server error

# Kill a container

 POST /containers/(id or name)/kill

Kill the container id

Query parameters:

    signal - Signal to send to the container: integer or string like SIGINT. When not set, SIGKILL is assumed and the call waits for the container to exit.

Status codes:

    204 – no error
    404 – no such container
    500 – server error

# Update a container

 POST /containers/(id or name)/update

Update configuration of one or more containers.

Status codes:

    200 – no error
    400 – bad parameter
    404 – no such container
    500 – server error

# Rename a container

 POST /containers/(id or name)/rename

Rename the container id to a new_name

Query parameters:

    name – new name for the container

Status codes:

    204 – no error
    404 – no such container
    409 - conflict name already assigned
    500 – server error

# Pause a container

 POST /containers/(id or name)/pause

Pause the container id

Status codes:

    204 – no error
    404 – no such container
    500 – server error

# Unpause a container

 POST /containers/(id or name)/unpause

Unpause the container id

Status codes:

    204 – no error
    404 – no such container
    500 – server error

# Attach to a container

 POST /containers/(id or name)/attach

Attach to the container id

Query parameters:

    detachKeys – Override the key sequence for detaching a container. Format is a single character [a-Z] or ctrl-<value> where <value> is one of: a-z, @, ^, [, , or _.
    logs – 1/True/true or 0/False/false, return logs. Default false.
    stream – 1/True/true or 0/False/false, return stream. Default false.
    stdin – 1/True/true or 0/False/false, if stream=true, attach to stdin. Default false.
    stdout – 1/True/true or 0/False/false, if logs=true, return stdout log, if stream=true, attach to stdout. Default false.
    stderr – 1/True/true or 0/False/false, if logs=true, return stderr log, if stream=true, attach to stderr. Default false.

Status codes:

    101 – no error, hints proxy about hijacking
    200 – no error, no upgrade header found
    400 – bad parameter
    404 – no such container

    500 – server error

# Attach to a container (websocket)

 GET /containers/(id or name)/attach/ws

Attach to the container id via websocket

Implements websocket protocol handshake according to RFC 6455

Query parameters:

    detachKeys – Override the key sequence for detaching a container. Format is a single character [a-Z] or ctrl-<value> where <value> is one of: a-z, @, ^, [, , or _.
    logs – 1/True/true or 0/False/false, return logs. Default false.
    stream – 1/True/true or 0/False/false, return stream. Default false.
    stdin – 1/True/true or 0/False/false, if stream=true, attach to stdin. Default false.
    stdout – 1/True/true or 0/False/false, if logs=true, return stdout log, if stream=true, attach to stdout. Default false.
    stderr – 1/True/true or 0/False/false, if logs=true, return stderr log, if stream=true, attach to stderr. Default false.

Status codes:

    200 – no error
    400 – bad parameter
    404 – no such container
    500 – server error

# Wait a container

 POST /containers/(id or name)/wait

Block until container id stops, then returns the exit code

Status codes:

    200 – no error
    404 – no such container
    500 – server error

# Remove a container

DELETE /containers/(id or name)

Remove the container id from the filesystem

Query parameters:

    v – 1/True/true or 0/False/false, Remove the volumes associated to the container. Default false.
    force - 1/True/true or 0/False/false, Kill then remove the container. Default false.

Status codes:

    204 – no error
    400 – bad parameter
    404 – no such container
    500 – server error

# Retrieving information about files and folders in a container

 HEAD /containers/(id or name)/archive

See the description of the X-Docker-Container-Path-Stat header in the following section.

# Get an archive of a filesystem resource in a container

 GET /containers/(id or name)/archive

Get a tar archive of a resource in the filesystem of container id.

Query parameters:

    path - resource in the container’s filesystem to archive. Required.

    If not an absolute path, it is relative to the container’s root directory. The resource specified by path must exist. To assert that the resource is expected to be a directory, path should end in / or /. (assuming a path separator of /). If path ends in /. then this indicates that only the contents of the path directory should be copied. A symlink is always resolved to its target.

        Note: It is not possible to copy certain system files such as resources under /proc, /sys, /dev, and mounts created by the user in the container.

Status codes:

    200 - success, returns archive of copied resource
    400 - client error, bad parameter, details in JSON response body, one of:
        must specify path parameter (path cannot be empty)
        not a directory (path was asserted to be a directory but exists as a file)
    404 - client error, resource not found, one of: – no such container (container id does not exist)
        no such file or directory (path does not exist)
    500 - server error

# Extract an archive of files or folders to a directory in a container

 PUT /containers/(id or name)/archive

Upload a tar archive to be extracted to a path in the filesystem of container id.

Query parameters:

    path - path to a directory in the container to extract the archive’s contents into. Required.

    If not an absolute path, it is relative to the container’s root directory. The path resource must exist.

    noOverwriteDirNonDir - If “1”, “true”, or “True” then it will be an error if unpacking the given content would cause an existing directory to be replaced with a non-directory and vice versa.

Status codes:

    200 – the content was extracted successfully
    400 - client error, bad parameter, details in JSON response body, one of:
        must specify path parameter (path cannot be empty)
        not a directory (path should be a directory but exists as a file)
        unable to overwrite existing directory with non-directory (if noOverwriteDirNonDir)
        unable to overwrite existing non-directory with directory (if noOverwriteDirNonDir)
    403 - client error, permission denied, the volume or container rootfs is marked as read-only.
    404 - client error, resource not found, one of: – no such container (container id does not exist)
        no such file or directory (path resource does not exist)
    500 – server error

Images
------

# List Images

 GET /images/json

Query parameters:

    all – 1/True/true or 0/False/false, default false
    filters – a JSON encoded value of the filters (a map[string][]string) to process on the images list. Available filters:
        dangling=true
        label=key or label="key=value" of an image label
        before=(<image-name>[:<tag>], <image id> or <image@digest>)
        since=(<image-name>[:<tag>], <image id> or <image@digest>)
    filter - only return images with the specified name

# Build image from a Dockerfile

 POST /build

Build an image from a Dockerfile

Query parameters:

    dockerfile - Path within the build context to the Dockerfile. This is ignored if remote is specified and points to an individual filename.
    t – A name and optional tag to apply to the image in the name:tag format. If you omit the tag the default latest value is assumed. You can provide one or more t parameters.
    remote – A Git repository URI or HTTP/HTTPS URI build source. If the URI specifies a filename, the file’s contents are placed into a file called Dockerfile.
    q – Suppress verbose build output.
    nocache – Do not use the cache when building the image.
    pull - Attempt to pull the image even if an older image exists locally.
    rm - Remove intermediate containers after a successful build (default behavior).
    forcerm - Always remove intermediate containers (includes rm).
    memory - Set memory limit for build.
    memswap - Total memory (memory + swap), -1 to enable unlimited swap.
    cpushares - CPU shares (relative weight).
    cpusetcpus - CPUs in which to allow execution (e.g., 0-3, 0,1).
    cpuperiod - The length of a CPU period in microseconds.
    cpuquota - Microseconds of CPU time that the container can get in a CPU period.
    buildargs – JSON map of string pairs for build-time variables. Users pass these values at build-time. Docker uses the buildargs as the environment context for command(s) run via the Dockerfile’s RUN instruction or for variable expansion in other Dockerfile instructions. This is not meant for passing secret values. Read more about the buildargs instruction
    shmsize - Size of /dev/shm in bytes. The size must be greater than 0. If omitted the system uses 64MB.

    labels – JSON map of string pairs for labels to set on the image.

    Request Headers:

    Content-type – Set to "application/tar".

    X-Registry-Config – A base64-url-safe-encoded Registry Auth Config JSON object with the following structure:

        {
            "docker.example.com": {
                "username": "janedoe",
                "password": "hunter2"
            },
            "https://index.docker.io/v1/": {
                "username": "mobydock",
                "password": "conta1n3rize14"
            }
        }

    This object maps the hostname of a registry to an object containing the “username” and “password” for that registry. Multiple registries may be specified as the build may be based on an image requiring authentication to pull from any arbitrary registry. Only the registry domain name (and port if not the default “443”) are required. However (for legacy reasons) the “official” Docker, Inc. hosted registry must be specified with both a “https://” prefix and a “/v1/” suffix even though Docker will prefer to use the v2 registry API.

Status codes:

    200 – no error
    500 – server error

# Create an image

 POST /images/create

Create an image either by pulling it from the registry or by importing it

Query parameters:

    fromImage – Name of the image to pull. The name may include a tag or digest. This parameter may only be used when pulling an image. The pull is cancelled if the HTTP connection is closed.
    fromSrc – Source to import. The value may be a URL from which the image can be retrieved or - to read the image from the request body. This parameter may only be used when importing an image.
    repo – Repository name given to an image when it is imported. The repo may include a tag. This parameter may only be used when importing an image.

    tag – Tag or digest.

    Request Headers:

    X-Registry-Auth – base64-encoded AuthConfig object, containing either login information, or a token

        Credential based login:

        {
            "username": "jdoe",
            "password": "secret",
            "email": "jdoe@acme.com"
        }

        Token based login:

        {
            "registrytoken": "9cbaf023786cd7..."
        }

Status codes:

    200 – no error
    500 – server error

# Inspect an image

 GET /images/(name)/json

Return low-level information on the image name

Status codes:

    200 – no error
    404 – no such image
    500 – server error

# Get the history of an image

 GET /images/(name)/history

Return the history of the image name

Status codes:

    200 – no error
    404 – no such image
    500 – server error

# Push an image on the registry

 POST /images/(name)/push

Push the image name on the registry

Query parameters:

    tag – The tag to associate with the image on the registry. This is optional.

Request Headers:

    X-Registry-Auth – base64-encoded AuthConfig object, containing either login information, or a token

        Credential based login:

        {
            "username": "jdoe",
            "password": "secret",
            "email": "jdoe@acme.com",
        }

        Identity token based login:

        {
            "identitytoken": "9cbaf023786cd7..."
        }

Status codes:

    200 – no error
    404 – no such image
    500 – server error

# Tag an image into a repository

 POST /images/(name)/tag

Tag the image name into a repository

Query parameters:

    repo – The repository to tag in
    tag - The new tag name

Status codes:

    201 – no error
    400 – bad parameter
    404 – no such image
    409 – conflict
    500 – server error

# Remove an image

DELETE /images/(name)

Remove the image name from the filesystem

Query parameters:

    force – 1/True/true or 0/False/false, default false
    noprune – 1/True/true or 0/False/false, default false

Status codes:

    200 – no error
    404 – no such image
    409 – conflict
    500 – server error

# Search images

 GET /images/search

Search for an image on Docker Hub.

    Note: The response keys have changed from API v1.6 to reflect the JSON sent by the registry server to the docker daemon’s request.

Query parameters:

    term – term to search
    limit – maximum returned search results
    filters – a JSON encoded value of the filters (a map[string][]string) to process on the images list. Available filters:
        stars=<number>
        is-automated=(true|false)
        is-official=(true|false)

Status codes:

    200 – no error
    500 – server error

Misc
----

# Check auth configuration

 POST /auth

Validate credentials for a registry and get identity token, if available, for accessing the registry without password.

Status codes:

    200 – no error
    204 – no error
    500 – server error

# Display system-wide information

 GET /info

Display system-wide information

Status codes:

    200 – no error
    500 – server error

# Show the docker version information

 GET /version

Show the docker version information

Status codes:

    200 – no error
    500 – server error

# Ping the docker server

 GET /\_ping

Ping the docker server

Status codes:

    200 - no error
    500 - server error

# Create a new image from a container’s changes

 POST /commit

Create a new image from a container’s changes

JSON parameters:

    config - the container’s configuration

Query parameters:

    container – source container
    repo – repository
    tag – tag
    comment – commit message
    author – author (e.g., “John Hannibal Smith <hannibal@a-team.com>“)
    pause – 1/True/true or 0/False/false, whether to pause the container before committing
    changes – Dockerfile instructions to apply while committing

Status codes:

    201 – no error
    404 – no such container
    500 – server error

# Monitor Docker’s events

 GET /events

Get container events from docker, either in real time via streaming, or via polling (using since).

Docker containers report the following events:

attach, commit, copy, create, destroy, detach, die, exec_create, exec_detach, exec_start, export, kill, oom, pause, rename, resize, restart, start, stop, top, unpause, update

Docker images report the following events:

delete, import, load, pull, push, save, tag, untag

Docker volumes report the following events:

create, mount, unmount, destroy

Docker networks report the following events:

create, connect, disconnect, destroy

Docker daemon report the following event:

reload

Query parameters:

    since – Timestamp used for polling
    until – Timestamp used for polling
    filters – A json encoded value of the filters (a map[string][]string) to process on the event list. Available filters:
        container=<string>; -- container to filter
        event=<string>; -- event to filter
        image=<string>; -- image to filter
        label=<string>; -- image and container label to filter
        type=<string>; -- either container or image or volume or network or daemon
        volume=<string>; -- volume to filter
        network=<string>; -- network to filter
        daemon=<string>; -- daemon name or id to filter

Status codes:

    200 – no error
    500 – server error

# Get a tarball containing all images in a repository

 GET /images/(name)/get

Get a tarball containing all images and metadata for the repository specified by name.

If name is a specific name and tag (e.g. ubuntu:latest), then only that image (and its parents) are returned. If name is an image ID, similarly only that image (and its parents) are returned, but with the exclusion of the ‘repositories’ file in the tarball, as there were no image names referenced.

See the image tarball format for more details.

Status codes:

    200 – no error
    500 – server error

# Get a tarball containing all images

 GET /images/get

Get a tarball containing all images and metadata for one or more repositories.

For each value of the names parameter: if it is a specific name and tag (e.g. ubuntu:latest), then only that image (and its parents) are returned; if it is an image ID, similarly only that image (and its parents) are returned and there would be no names referenced in the ‘repositories’ file for this image ID.

See the image tarball format for more details.

Status codes:

    200 – no error
    500 – server error

# Load a tarball with a set of images and tags into docker

 POST /images/load

Load a set of images and tags into a Docker repository. See the image tarball format for more details.

Status codes:

    200 – no error
    500 – server error

# Exec Create

 POST /containers/(id or name)/exec

Sets up an exec instance in a running container id

JSON parameters:

    AttachStdin - Boolean value, attaches to stdin of the exec command.
    AttachStdout - Boolean value, attaches to stdout of the exec command.
    AttachStderr - Boolean value, attaches to stderr of the exec command.
    DetachKeys – Override the key sequence for detaching a container. Format is a single character [a-Z] or ctrl-<value> where <value> is one of: a-z, @, ^, [, , or _.
    Tty - Boolean value to allocate a pseudo-TTY.
    Cmd - Command to run specified as a string or an array of strings.

Status codes:

    201 – no error
    404 – no such container
    409 - container is paused
    500 - server error

# Exec Start

 POST /exec/(id)/start

Starts a previously set up exec instance id. If detach is true, this API returns after starting the exec command. Otherwise, this API sets up an interactive session with the exec command.

JSON parameters:

    Detach - Detach from the exec command.
    Tty - Boolean value to allocate a pseudo-TTY.

Status codes:

    200 – no error
    404 – no such exec instance

    409 - container is paused

    Stream details: Similar to the stream behavior of  POST /containers/(id or name)/attach API

# Exec Resize

 POST /exec/(id)/resize

Resizes the tty session used by the exec command id. The unit is number of characters. This API is valid only if tty was specified as part of creating and starting the exec command.

Query parameters:

    h – height of tty session
    w – width

Status codes:

    201 – no error
    404 – no such exec instance

# Exec Inspect

 GET /exec/(id)/json

Return low-level information about the exec command id.

Status codes:

    200 – no error
    404 – no such exec instance
    500 - server error

Volumes
-------

# List volumes

 GET /volumes

Query parameters:

    filters - JSON encoded value of the filters (a map[string][]string) to process on the volumes list. Available filters:
        name=<volume-name> Matches all or part of a volume name.
        dangling=<boolean> When set to true (or 1), returns all volumes that are “dangling” (not in use by a container). When set to false (or 0), only volumes that are in use by one or more containers are returned.
        driver=<volume-driver-name> Matches all or part of a volume driver name.

Status codes:

    200 - no error
    500 - server error

# Create a volume

 POST /volumes/create

Create a volume

Status codes:

    201 - no error
    500 - server error

JSON parameters:

    Name - The new volume’s name. If not specified, Docker generates a name.
    Driver - Name of the volume driver to use. Defaults to local for the name.
    DriverOpts - A mapping of driver options and values. These options are passed directly to the driver and are driver specific.
    Labels - Labels to set on the volume, specified as a map: {"key":"value" [,"key2":"value2"]}

# Inspect a volume

 GET /volumes/(name)

Return low-level information on the volume name

Status codes:

    200 - no error
    404 - no such volume
    500 - server error

# Remove a volume

DELETE /volumes/(name)

Instruct the driver to remove the volume (name).

Status codes:

    204 - no error
    404 - no such volume or volume driver
    409 - volume is in use and cannot be removed
    500 - server error

Networks
--------

# List networks

 GET /networks

Query parameters:

    filters - JSON encoded network list filter. The filter value is one of:
        driver=<driver-name> Matches a network’s driver.
        id=<network-id> Matches all or part of a network id.
        label=<key> or label=<key>=<value> of a network label.
        name=<network-name> Matches all or part of a network name.
        type=["custom"|"builtin"] Filters networks by type. The custom keyword returns all user-defined networks.

Status codes:

    200 - no error
    500 - server error

# Inspect network

 GET /networks/<network-id>

Status codes:

    200 - no error
    404 - network not found

# Create a network

 POST /networks/create

Create a network

Status codes:

    201 - no error
    404 - plugin not found
    500 - server error

JSON parameters:

    Name - The new network’s name. this is a mandatory field
    CheckDuplicate - Requests daemon to check for networks with same name
    Driver - Name of the network driver plugin to use. Defaults to bridge driver
    Internal - Restrict external access to the network
    IPAM - Optional custom IP scheme for the network
    EnableIPv6 - Enable IPv6 on the network
    Options - Network specific options to be used by the drivers
    Labels - Labels to set on the network, specified as a map: {"key":"value" [,"key2":"value2"]}

# Connect a container to a network

 POST /networks/(id)/connect

Connect a container to a network

Status codes:

    200 - no error
    404 - network or container is not found
    500 - Internal Server Error

JSON parameters:

    container - container-id/name to be connected to the network

# Disconnect a container from a network

 POST /networks/(id)/disconnect

Disconnect a container from a network

Status codes:

    200 - no error
    404 - network or container not found
    500 - Internal Server Error

JSON parameters:

    Container - container-id/name to be disconnected from a network
    Force - Force the container to disconnect from a network

# Remove a network

DELETE /networks/(id)

Instruct the driver to remove the network (id).

