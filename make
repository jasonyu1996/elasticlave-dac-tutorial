#!/bin/bash

CURDIR="$(readlink -f $(dirname $0))"

"${CURDIR}"/scripts/docker-run ./scripts/build

