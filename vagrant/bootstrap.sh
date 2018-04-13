#!/usr/bin/env bash

sudo apt-get update
sudo apt-get install -y gcc make tmux zsh

if [ -z "$TRAVIS"]; then
    sudo apt-get install -y ruby2.3 ruby2.3-dev
fi

# Change directory to the checkout root
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/..

sudo gem install bundle
bundle install