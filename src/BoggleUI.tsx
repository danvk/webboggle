import React from "react";
import useSWR from "swr";
import { BoggleWord, getTrieForWordlist, getWordsOnBoard } from "./boggle";
import { Trie } from "./boggle-wasm";
import { BoggleGrid } from "./BoggleGrid";
import classNames from "classnames";

export interface BoggleUIProps {
  wordlist: string;
  board: string;
  multiboggle: boolean;
}

interface BoggleWordListProps {
  words: BoggleWord[];
  selectedIndex: number | null;
  setSelectedIndex: (index: number | null) => void;
}

function BoggleWordList(props: BoggleWordListProps) {
  const { words, selectedIndex, setSelectedIndex } = props;
  return (
    <ol>
      {words.map((word, i) => (
        <li
          key={i}
          // TODO: this creates a ton of functions
          onMouseOver={() => setSelectedIndex(i)}
          className={classNames({ selected: i === selectedIndex })}
        >
          {word.word.replace(/q/g, "qu")}
        </li>
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
  const [selectedIndex, setSelectedIndex] = React.useState<number | null>(null);
  const selectedPath =
    selectedIndex !== null ? words[selectedIndex].path : null;

  React.useEffect(() => {
    setSelectedIndex(null);
  }, [board, multiboggle, trie]);

  return (
    <div>
      <BoggleGrid board={board} selectedPath={selectedPath} />
      <div>{points} points</div>
      <BoggleWordList
        words={words}
        selectedIndex={selectedIndex}
        setSelectedIndex={setSelectedIndex}
      />
    </div>
  );
}
