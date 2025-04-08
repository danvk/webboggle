# Web-based Boggle Solver

This repo wraps a web UI around a WASM build of the code in [hybrid-boggle]. Try it out at [danvk.org/boggle].

Quickstart:

```
pnpm i
pnpm dev
```

To deploy:

```
pnpm build
rm -rf ../danvk.github.io/boggle
cp -r dist ~/github/danvk.github.io/boggle
```

To update the WASM binary:

```
cd ../hybrid-boggle
emcc -lembind -sALLOW_MEMORY_GROWTH -sEXPORTED_FUNCTIONS=FS -sFORCE_FILESYSTEM=1 -o wasm/boggle.js cpp/wasm_boggle.cc cpp/trie.cc --emit-tsd boggle.d.ts
mv wasm/boggle.{js,wasm} ../webboggle/public/wasm
mv wasm/boggle.d.ts ../webboggle/src/boggle-wasm.ts
```

[hybrid-boggle]: https://github.com/danvk/hybrid-boggle/
[danvk.org/boggle]: https://www.danvk.org/boggle/
