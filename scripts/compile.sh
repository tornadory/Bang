#!/bin/bash

cd "$(dirname "$0")"
cd .. # Go to root directory

source "scripts/captureArgs.sh"
source "scripts/compileExecutables.sh" "${RELEASE_OR_DEBUG}"

