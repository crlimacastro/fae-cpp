#!/bin/sh

# Configure with Emscripten
emcmake cmake -B out/build/web

# Build with the generated build system
cmake --build out/build/web

# Start a HTTP server (The address, most likely localhost:8080, will be pointing to the root of the project. Navigate desired example's HTML file to see the result.)
npx http-server