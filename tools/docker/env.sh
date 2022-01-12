#!/bin/bash

cd $(dirname "$(realpath $0)")/../..
docker build -f tools/docker/Dockerfile -t tepee-dev . \
	&& docker run -v $(pwd):/home/src -p 80:80 -it tepee-dev /bin/bash
