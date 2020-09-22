FROM debian:buster-slim

RUN apt-get update
RUN apt-get install -y build-essential git cmake libboost-all-dev libcln-dev libgmp-dev libginac-dev automake libglpk-dev libhwloc-dev libz3-dev libxerces-c-dev libeigen3-dev

WORKDIR /home
RUN mkdir storm
