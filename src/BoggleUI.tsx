import React from "react";
import useSWR from "swr";
import {
  BoggleWord,
  getTrieForWordlist,
  getWordsOnBoard,
  SCORES,
} from "./boggle";
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

type SortOrder = "alphabetical" | "length";

function BoggleWordList(props: BoggleWordListProps) {
  const { words, selectedIndex, setSelectedIndex } = props;
  const [sortOrder, setSortOrder] = React.useState<SortOrder>("length");
  const displayWords = words.map((w, i) => ({
    i,
    word: w.word.replace(/q/g, "qu"),
  }));
  if (sortOrder == "length") {
    displayWords.sort(
      (w1, w2) =>
        w2.word.length - w1.word.length || w1.word.localeCompare(w2.word)
    );
  } else {
    displayWords.sort((w1, w2) => w1.word.localeCompare(w2.word));
  }

  return (
    <>
      <select
        value={sortOrder}
        onChange={(e) => setSortOrder(e.currentTarget.value as SortOrder)}
      >
        <option value="alphabetical">Alphabetical</option>
        <option value="length">Length</option>
      </select>
      <ol onMouseOut={() => setSelectedIndex(null)}>
        {displayWords.map((word) => (
          <li
            key={word.i}
            // TODO: this creates a ton of functions
            onMouseOver={() => setSelectedIndex(word.i)}
            className={classNames({ selected: word.i === selectedIndex })}
          >
            {word.word}
            {word.word.length > 4 ? ` (${SCORES[word.word.length]})` : null}
          </li>
        ))}
      </ol>
    </>
  );
}

export const BoggleUI = React.memo((props: BoggleUIProps) => {
  const { wordlist } = props;

  const { data: trie, isLoading, error } = useSWR(wordlist, getTrieForWordlist);

  if (isLoading) return "Loading...";
  if (error) {
    console.error(error);
    return "Error!";
  }
  return <BoggleUIWithTrie {...props} trie={trie!} />;
});

function BoggleUIWithTrie(props: BoggleUIProps & { trie: Trie }) {
  const { board, multiboggle, trie } = props;
  const [points, words] = React.useMemo(
    () => getWordsOnBoard(trie, board, multiboggle),
    [board, multiboggle, trie]
  );
  const [selectedIndex, setSelectedIndex] = React.useState<number | null>(null);
  const selectedPath =
    selectedIndex !== null ? words[selectedIndex]?.path ?? null : null;

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
