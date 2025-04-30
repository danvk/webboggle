// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
declare namespace RuntimeExports {
    let FS_createPath: any;
    function FS_createDataFile(parent: any, name: any, fileData: any, canRead: any, canWrite: any, canOwn: any): void;
    function FS_createPreloadedFile(parent: any, name: any, url: any, canRead: any, canWrite: any, onload: any, onerror: any, dontCreateFile: any, canOwn: any, preFinish: any): void;
    function FS_unlink(path: any): any;
    let FS_createLazyFile: any;
    let FS_createDevice: any;
    let addRunDependency: any;
    let removeRunDependency: any;
}
interface WasmModule {
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  clone(): this;
}
export interface Trie extends ClassHandle {
  num_nodes(): number;
  size(): number;
}

export interface VectorInt extends ClassHandle {
  push_back(_0: number): void;
  resize(_0: number, _1: number): void;
  size(): number;
  get(_0: number): number | undefined;
  set(_0: number, _1: number): boolean;
}

export interface VectorVectorInt extends ClassHandle {
  push_back(_0: VectorInt): void;
  resize(_0: number, _1: VectorInt): void;
  size(): number;
  get(_0: number): VectorInt | undefined;
  set(_0: number, _1: VectorInt): boolean;
}

export interface Boggler33 extends ClassHandle {
  find_words(_0: EmbindString, _1: boolean): VectorVectorInt;
}

export interface Boggler44 extends ClassHandle {
  find_words(_0: EmbindString, _1: boolean): VectorVectorInt;
}

export interface Boggler55 extends ClassHandle {
  find_words(_0: EmbindString, _1: boolean): VectorVectorInt;
}

interface EmbindModule {
  Trie: {
    new(): Trie;
    CreateFromFile(_0: EmbindString): Trie;
  };
  VectorInt: {
    new(): VectorInt;
  };
  VectorVectorInt: {
    new(): VectorVectorInt;
  };
  Boggler33: {
    new(_0: Trie | null): Boggler33;
  };
  Boggler44: {
    new(_0: Trie | null): Boggler44;
  };
  Boggler55: {
    new(_0: Trie | null): Boggler55;
  };
}

export type MainModule = WasmModule & typeof RuntimeExports & EmbindModule;
