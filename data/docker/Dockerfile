FROM base/archlinux:latest

RUN pacman --noconfirm --needed -Sy archlinux-keyring
RUN pacman --noconfirm --needed -Sy base-devel git expac yajl perl
RUN pacman-db-upgrade
RUN pacman --noconfirm --needed -Syu
RUN pacman --noconfirm --needed -Sy libunistring

RUN useradd -m -s /bin/bash aur && echo "aur ALL = NOPASSWD: /usr/bin/pacman" >> /etc/sudoers
RUN mkdir /tmp/pacaur && \
    chown -R aur:aur /tmp/pacaur && \
    chmod -R 771 /tmp/pacaur


## cloning and install pacaur
USER aur
ENV PATH $PATH:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl
WORKDIR /tmp/pacaur
RUN git clone https://aur.archlinux.org/cower.git
WORKDIR /tmp/pacaur/cower
RUN makepkg --noconfirm --skippgpcheck
USER root 
WORKDIR /tmp/pacaur/cower
RUN pacman --noconfirm -U *.tar.xz

## cloning and install pacaur
USER aur 
WORKDIR /tmp/pacaur
RUN git clone https://aur.archlinux.org/pacaur.git
WORKDIR /tmp/pacaur/pacaur
RUN sed -i -e '/build()/,+4d' -e '/man8/,+1d' PKGBUILD
RUN makepkg --noconfirm --skippgpcheck
USER root 
WORKDIR /tmp/pacaur/pacaur
RUN pacman --noconfirm -U *.tar.xz

USER aur
RUN pacaur -Syu --noedit --noconfirm boost-libs 
RUN pacaur -Syu --noedit --noconfirm poco

RUN pacaur -Syu --noedit --noconfirm zsh
RUN touch ~/.zshrc
