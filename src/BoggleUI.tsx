import React from "react";
import useSWR from "swr";
import {
  BoggleWord,
  getTrieForWordlist,
  getWordsOnBoard,
  makeBoard,
  SCORES,
} from "./boggle";
import { Trie } from "./boggle-wasm";
import { BoggleGrid } from "./BoggleGrid";
import classNames from "classnames";

export interface BoggleUIProps {
  dims: 33 | 44 | 55;
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

  const [numToShow, setNumToShow] = React.useState(250);
  React.useEffect(() => {
    setNumToShow(250);
  }, [words]);
  const showMore = () => setNumToShow(numToShow + 250);

  const handleMouseEnter: React.MouseEventHandler = (e) => {
    setSelectedIndex(Number(e.currentTarget.getAttribute("data-index")));
  };

  return (
    <>
      <select
        value={sortOrder}
        onChange={(e) => setSortOrder(e.currentTarget.value as SortOrder)}
      >
        <option value="alphabetical">Alphabetical</option>
        <option value="length">Length</option>
      </select>
      <ol>
        {displayWords.slice(0, numToShow).map((word) => (
          <li
            key={word.i}
            data-index={word.i}
            // TODO: this creates a ton of functions
            onMouseEnter={handleMouseEnter}
            className={classNames({ selected: word.i === selectedIndex })}
          >
            {word.word}
            {word.word.length > 4 ? ` (${SCORES[word.word.length]})` : null}
          </li>
        ))}
      </ol>
      {numToShow < words.length ? (
        <button onClick={showMore}>Show More</button>
      ) : null}
    </>
  );
}

export const BoggleUI = React.memo((props: BoggleUIProps) => {
  const { board, dims, wordlist } = props;

  const boardArray = makeBoard(board, dims);
  const { data: trie, isLoading, error } = useSWR(wordlist, getTrieForWordlist);

  if (isLoading) return "Loading...";
  if (error) {
    console.error(error);
    return "Error!";
  }
  return <BoggleUIWithTrie {...props} trie={trie!} board={boardArray} />;
});

function BoggleUIWithTrie(props: BoggleUIProps & { trie: Trie }) {
  const { board, dims, multiboggle, trie } = props;
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
      <BoggleGrid board={board} dims={dims} selectedPath={selectedPath} />
      <div>
        {points} points, {words.length} words
      </div>
      <BoggleWordList
        words={words}
        selectedIndex={selectedIndex}
        setSelectedIndex={setSelectedIndex}
      />
    </div>
  );
}
