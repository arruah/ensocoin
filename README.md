[Website](https://www.REPLACE_1_LINK)  | [Download](https://www.REPLACE_1_LINK/download) | [Setup](doc/README.md)  |  [Xthin](doc/bu-xthin.md)  |  [Xpedited](doc/bu-xpedited-forwarding.md)  |   [Miner](doc/miner.md)

[![Build Status](https://travis-ci.org/BitcoinUnlimited/BitcoinUnlimited.svg?branch=release)](https://travis-ci.org/BitcoinUnlimited/BitcoinUnlimited)

What is Bitcoin?
=====================================

Bitcoin is an experimental new digital currency that enables instant payments to
anyone, anywhere in the world. Bitcoin uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. REPLACE_1_PROJNAME_MD is the name of open source
software which enables the use of this currency.

For more information, as well as an immediately useable, binary version of
the REPLACE_1_PROJNAME_MD software, see https://www.REPLACE_1_LINK/download, or read the
[original whitepaper](https://www.REPLACE_1_LINK/resources/bitcoin.pdf).

License
-------

REPLACE_1_PROJNAME_MD is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

What is REPLACE_1_PROJNAME_MD?
=====================================

REPLACE_1_PROJNAME_MD is an implementation of the Bitcoin client software that is based on Bitcoin Core.
However, REPLACE_1_PROJNAME_MD has a very different philosophy than Core.

It follows a philosophy and is administered by a formal process described in the [Articles of Federation](http://www.REPLACE_1_LINK/resources/BUarticles.pdf).
In short, we believe in market-driven decision making, emergent consensus, and giving our users choices.

Quick installation Instructions
====================================

If you're running an Ubuntu system:

```sh
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:bitcoin-unlimited/bu-ppa
sudo apt-get update
sudo apt-get install bitcoind bitcoin-qt
```
If you're compiling from source:

```sh
sudo apt-get install git build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils libboost-all-dev
## optional: only needed if you want bitcoin-qt
sudo apt-get install libqt4-dev libprotobuf-dev protobuf-compiler libqrencode-dev
## optional: only needed if your wallet use the old format
sudo apt-get install software-properties-common

## this not needed if your wallet will use the new
## format, ot if you're not going to use a wallet at all
sudo add-apt-repository ppa:bitcoin-unlimited/bu-ppa
sudo apt-get update
sudo apt-get install libdb4.8-dev libdb4.8++-dev

mkdir -p ~/src
cd ~/src
git clone https://REPLACE_2_LINK bu-src
cd bu-src
./autogen.sh
./configure
make
sudo make install
```

For more detailed explanations on how compile from source just look at doc/build-*.md files (e.g. [here](doc/quick-install.md))
