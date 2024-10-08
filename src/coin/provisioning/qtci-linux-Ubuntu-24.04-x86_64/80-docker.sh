#!/usr/bin/env bash
# Copyright (C) 2023 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

set -e

PROVISIONING_DIR="$(dirname "$0")/../"
# shellcheck source=../common/unix/common.sourced.sh
source "$PROVISIONING_DIR"/common/unix/common.sourced.sh
# shellcheck source=../common/unix/DownloadURL.sh
source "$PROVISIONING_DIR"/common/unix/DownloadURL.sh

localRepo=http://ci-files01-hki.ci.qt.io/input/docker
# upstreamRepo=https://download.docker.com/linux/ubuntu/dists/bionic/pool/stable/amd64
#upstreamRepo=https://download.docker.com/linux/ubuntu/dists/jammy/pool/stable/amd64
upstreamRepo=https://download.docker.com/linux/ubuntu/dists/noble/pool/stable/amd64
echo '
    57c62933ad2dc07a3a79efd5b464ea84fa80c773 containerd.io_1.6.31-1_amd64.deb
    cbcd536345e052b9221a0240caa451e1b687a05e docker-ce_26.1.0-1~ubuntu.24.04~noble_amd64.deb
    6f0989763692b88b748444174e70b6069c781533 docker-ce-cli_26.1.0-1~ubuntu.24.04~noble_amd64.deb
' \
    | xargs -n2 | while read -r sha f
do
    DownloadURL "$localRepo/$f" "$upstreamRepo/$f" "$sha"
done

sudo apt-get -y install  ./containerd.io_*.deb ./docker-ce_*.deb ./docker-ce-cli_*.deb
rm -f                    ./containerd.io_*.deb ./docker-ce_*.deb ./docker-ce-cli_*.deb

sudo usermod -a -G docker "$USER"
sudo docker --version

# Download and install the docker-compose extension from https://github.com/docker/compose/releases
f=docker-compose-$(uname -s)-$(uname -m)
DownloadURL  \
    "$localRepo/$f-1.24.1"  \
    "https://github.com/docker/compose/releases/download/1.24.1/$f" \
    cfb3439956216b1248308141f7193776fcf4b9c9b49cbbe2fb07885678e2bb8a
sudo install -m 755 ./docker-compose* /usr/local/bin/docker-compose
sudo docker-compose --version
rm ./docker-compose*

# Install Avahi to discover Docker containers in the test network
sudo apt-get install avahi-daemon -y

# Add registry mirror for docker images
sudo tee -a /etc/docker/daemon.json <<"EOF"
{
  "registry-mirrors": ["http://repo-clones.ci.qt.io:5000"]
}
EOF

echo "Restart Docker"
sudo systemctl daemon-reload
sudo systemctl restart docker

# Start testserver provisioning
sudo "$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")/../common/shared/testserver/docker_testserver.sh"

