/** TypeScript wrapper around Boggle WASM */

import { MainModule, Trie } from "./boggle-wasm";

declare let modulePromise: Promise<MainModule>;
declare let Module: MainModule;

export async function getModule(): Promise<MainModule> {
  return modulePromise;
}

declare const loadWordlist: (networkPath: string) => Promise<string>;

export async function getTrieForWordlist(networkPath: string): Promise<Trie> {
  const [module, wordlistPath] = await Promise.all([
    getModule(),
    loadWordlist(networkPath),
  ]);
  const startMs = performance.now();
  const trie = module.Trie.CreateFromFile(wordlistPath);
  const elapsedMs = performance.now() - startMs;
  console.log("create trie", elapsedMs, "ms");
  return trie;
}

export interface BoggleWord {
  word: string;
  path: number[];
}

//                 1, 2, 3, 4, 5, 6, 7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
const scores = [
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
    score += scores[path.size() + numQ];
    boggleWords.push({ word, path: indices });
  }
  console.log("Score: ", score);

  return [score, boggleWords];
}
