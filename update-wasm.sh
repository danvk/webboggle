#!/bin/bash
set -o errexit

cd ../hybrid-boggle
./wasm/build-wasm.sh
mv wasm/boggle.{js,wasm} ../webboggle/public/wasm
mv wasm/boggle-wasm.ts ../webboggle/src/
