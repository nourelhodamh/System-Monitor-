# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.191.1/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-11, debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT= ubuntu-20.04
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update \
&& apt-get -y install libncurses5-dev libncursesw5-dev
RUN apt-get -y install clang

