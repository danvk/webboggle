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
```

[hybrid-boggle]: https://github.com/danvk/hybrid-boggle/
[danvk.org/boggle]: https://www.danvk.org/boggle/

(below is leftover stuff from `pnpm create vite app --template react-ts`)

## Expanding the ESLint configuration

If you are developing a production application, we recommend updating the configuration to enable type-aware lint rules:

```js
export default tseslint.config({
  extends: [
    // Remove ...tseslint.configs.recommended and replace with this
    ...tseslint.configs.recommendedTypeChecked,
    // Alternatively, use this for stricter rules
    ...tseslint.configs.strictTypeChecked,
    // Optionally, add this for stylistic rules
    ...tseslint.configs.stylisticTypeChecked,
  ],
  languageOptions: {
    // other options...
    parserOptions: {
      project: ['./tsconfig.node.json', './tsconfig.app.json'],
      tsconfigRootDir: import.meta.dirname,
    },
  },
})
```

You can also install [eslint-plugin-react-x](https://github.com/Rel1cx/eslint-react/tree/main/packages/plugins/eslint-plugin-react-x) and [eslint-plugin-react-dom](https://github.com/Rel1cx/eslint-react/tree/main/packages/plugins/eslint-plugin-react-dom) for React-specific lint rules:

```js
// eslint.config.js
import reactX from 'eslint-plugin-react-x'
import reactDom from 'eslint-plugin-react-dom'

export default tseslint.config({
  plugins: {
    // Add the react-x and react-dom plugins
    'react-x': reactX,
    'react-dom': reactDom,
  },
  rules: {
    // other rules...
    // Enable its recommended typescript rules
    ...reactX.configs['recommended-typescript'].rules,
    ...reactDom.configs.recommended.rules,
  },
})
```
