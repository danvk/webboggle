import React from "react";
import useSWR from "swr";
import { BoggleWord, getTrieForWordlist, getWordsOnBoard } from "./boggle";
import { Trie } from "./boggle-wasm";
import { BoggleGrid } from "./BoggleGrid";

export interface BoggleUIProps {
  wordlist: string;
  board: string;
  multiboggle: boolean;
}

function BoggleWordList(props: { words: BoggleWord[] }) {
  return (
    <ol>
      {props.words.map((word, i) => (
        <li key={i}>{word.word}</li>
      ))}
    </ol>
  );
}

export function BoggleUI(props: BoggleUIProps) {
  const { wordlist } = props;

  const { data: trie, isLoading, error } = useSWR(wordlist, getTrieForWordlist);

  if (isLoading) return "Loading...";
  if (error) {
    console.error(error);
    return "Error!";
  }
  return <BoggleUIWithTrie {...props} trie={trie!} />;
}

function BoggleUIWithTrie(props: BoggleUIProps & { trie: Trie }) {
  const { board, multiboggle, trie } = props;
  const [points, words] = React.useMemo(
    () => getWordsOnBoard(trie, board, multiboggle),
    [board, multiboggle, trie]
  );

  return (
    <div>
      <BoggleGrid board={board} />
      <div>{points} points</div>
      <BoggleWordList words={words} />
    </div>
  );
}
