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

//                 1, 2, 3, 4, 5, 6, 7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
export const SCORES = [
  0, 0, 0, 1, 1, 2, 3, 5, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
];

export function getWordsOnBoard(
  trie: Trie,
  board: string,
  multiboggle: boolean
): [number, BoggleWord[]] {
  // If you have a trie, then the WASM module must have already loaded.
  const boggler = new Module.Boggler(trie);
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

export function makeBoard44(board: string) {
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
  } else if (board.length == 9) {
    return (
      board.slice(0, 3) +
      "." +
      board.slice(3, 6) +
      "." +
      board.slice(6, 9) +
      "." +
      "...."
    );
  }
  throw new Error("Invalid board length");
}

export function isValidBoard(board: string) {
  return board.length === 9 || board.length === 12 || board.length === 16;
}
