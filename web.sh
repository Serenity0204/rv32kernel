#!/bin/bash

set -e

echo "Running web Makefile..."
cd web
make "$@"
