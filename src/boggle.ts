/** TypeScript wrapper around Boggle WASM */

import { MainModule, Trie } from "./boggle-wasm";

declare let modulePromise: Promise<MainModule>;
declare let Module: MainModule;

export async function getModule(): Promise<MainModule> {
  return modulePromise;
}

declare const loadWordlist: (networkPath: string) => Promise<string>;

interface CachedTrie {
  wordlistPath: string;
  trie: Trie;
}
let cachedTrie: CachedTrie | null = null;

export async function getTrieForWordlist(networkPath: string): Promise<Trie> {
  const [module, wordlistPath] = await Promise.all([
    getModule(),
    loadWordlist(networkPath),
  ]);
  if (cachedTrie?.wordlistPath == wordlistPath) {
    return cachedTrie.trie;
  }
  if (cachedTrie) {
    cachedTrie.trie.delete();
    cachedTrie = null;
  }
  const startMs = performance.now();
  const trie = module.Trie.CreateFromFile(wordlistPath);
  const elapsedMs = performance.now() - startMs;
  console.log("create trie", elapsedMs, "ms");
  cachedTrie = { wordlistPath, trie };
  return trie;
}

export interface BoggleWord {
  word: string;
  path: number[];
}

//   1, 2, 3, 4, 5, 6, 7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
export const SCORES = [
  0, 0, 0, 1, 1, 2, 3, 5, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
];

function getBogglerForSize(len: number) {
  switch (len) {
    case 9:
      return Module.Boggler33;
    case 16:
      return Module.Boggler44;
    case 25:
      return Module.Boggler55;
  }
  throw new Error("Invalid boggle size");
}

export function getWordsOnBoard(
  trie: Trie,
  board: string,
  multiboggle: boolean
): [number, BoggleWord[]] {
  // If you have a trie, then the WASM module must have already loaded.
  const Boggler = getBogglerForSize(board.length);
  const boggler = new Boggler(trie);
  const startMs = performance.now();
  const words = boggler.find_words(board, multiboggle);
  const elapsedMs = performance.now() - startMs;
  console.log("find words", elapsedMs, "ms");

  const boggleWords: BoggleWord[] = [];
  let score = 0;
  for (let i = 0; i < words.size(); i++) {
    const path = words.get(i)!;
    const indices = Array(path.size())
      .fill(null)
      .map((_, i) => path.get(i)) as number[];
    const word = indices.map((i) => board[i]).join("");
    const numQ = indices.filter((i) => board[i] === "q").length;
    score += SCORES[path.size() + numQ];
    boggleWords.push({ word, path: indices });
  }
  console.log("Score: ", score);

  return [score, boggleWords];
}

// TODO: error handling
export function makeBoard(board: string, dims: 33 | 44 | 55) {
  if (dims === 33) {
    if (board.length === 9) {
      return board;
    }
    // TODO: downsize 4x4 board
  } else if (dims === 44) {
    if (board.length == 16) {
      return board;
    } else if (board.length == 12) {
      const out = Array(16).fill(".");
      for (let i = 0; i < board.length; i++) {
        const x = i >> 2;
        const y = i % 4;
        out[4 * y + x] = board.charAt(i);
      }
      return out.join("");
    }
  } else if (dims === 55) {
    if (board.length === 25) {
      return board;
    }
  }
  throw new Error("Invalid board length");
}

export function isValidBoard(board: string) {
  return (
    board.length === 9 ||
    board.length === 12 ||
    board.length === 16 ||
    board.length === 25
  );
}

export function range(n: number) {
  return [...Array(n)].map((_, i) => i);
}

export function parseBoard(board: string, numCols: number) {
  return range(numCols).map((y) =>
    board.slice(y * numCols, (y + 1) * numCols).split("")
  );
}

export function adjustBoard(
  board: string,
  oldDims: 33 | 44 | 55,
  newDims: 33 | 44 | 55
) {
  const oldSize = oldDims % 10;
  const oldGrid = parseBoard(board, oldSize);
  const newSize = newDims % 10;
  const newGrid = range(newSize).map(() => range(newSize).map(() => "z"));
  for (const [y, row] of oldGrid.entries()) {
    for (const [x, v] of row.entries()) {
      if (y < newSize && x < newSize) {
        newGrid[y][x] = v;
      }
    }
  }
  return newGrid.map((row) => row.join("")).join("");
}
