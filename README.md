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
./update-wasm.sh
```

[hybrid-boggle]: https://github.com/danvk/hybrid-boggle/
[danvk.org/boggle]: https://www.danvk.org/boggle/
