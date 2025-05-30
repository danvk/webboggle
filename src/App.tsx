import { FormEvent } from "react";
import { BoggleUI } from "./BoggleUI";
import React from "react";

import "./App.css";
import "./Boggle.css";
import { useLightlyEncodedSearchParams } from "./router";
import { adjustBoard, isValidBoard } from "./boggle";

const WORDLISTS = [
  { key: "enable2k", display: "ENABLE2K", path: "wordlists/enable2k.txt" },
  { key: "naspa23", display: "NASPA23", path: "wordlists/naspa2023.txt" },
  { key: "ospd5", display: "OSPD5", path: "wordlists/ospd5.txt" },
  { key: "sowpods", display: "SOWPODS", path: "wordlists/sowpods.txt" },
  { key: "twl06", display: "TWL06", path: "wordlists/twl06.txt" },
  { key: "yawl", display: "YAWL", path: "wordlists/yawl.txt" },
];

interface UrlState {
  board: string;
  wordlist: string;
  multiboggle: boolean;
  dims: 33 | 44 | 55;
}
const DEFAULT_STATE: UrlState = {
  board: "perslatgsineters",
  wordlist: "enable2k",
  multiboggle: false,
  dims: 44,
};

function useUrlState() {
  const [searchParams, setSearchParams] = useLightlyEncodedSearchParams();

  const state = React.useMemo(() => {
    const s = {
      ...DEFAULT_STATE,
    };
    const board = searchParams.get("board");
    if (board) {
      s.board = board;
    }
    const wordlist = searchParams.get("wordlist");
    if (wordlist) {
      s.wordlist = wordlist;
    }
    const multiboggle = searchParams.get("multiboggle");
    if (multiboggle) {
      s.multiboggle = true;
    }
    const dims = searchParams.get("dims");
    if (dims == "33") {
      s.dims = 33;
    } else if (dims == "44") {
      s.dims = 44;
    } else if (dims == "55") {
      s.dims = 55;
    } else {
      if (s.board.length == 9) {
        s.dims = 33;
      } else if (s.board.length == 16 || s.board.length == 12) {
        s.dims = 44;
      } else if (s.board.length == 25) {
        s.dims = 55;
      }
    }
    console.log(s);
    return s;
  }, [searchParams]);

  const setState = React.useCallback(
    (newState: UrlState) => {
      const params: Partial<Record<keyof UrlState, string>> = {};
      if (newState.board !== DEFAULT_STATE.board) {
        params.board = newState.board;
      }
      if (newState.wordlist !== DEFAULT_STATE.wordlist) {
        params.wordlist = newState.wordlist;
      }
      if (newState.multiboggle !== DEFAULT_STATE.multiboggle) {
        params.multiboggle = "1";
      }
      if (newState.dims !== DEFAULT_STATE.dims) {
        params.dims = String(newState.dims);
      }
      setSearchParams(params);
    },
    [setSearchParams]
  );

  return [state, setState] as const;
}

function App() {
  const [urlState, setUrlState] = useUrlState();
  const { board, dims, wordlist, multiboggle } = urlState;

  const [transientBoard, setTransientBoard] = React.useState(board);
  React.useEffect(() => {
    setTransientBoard(board);
  }, [board]);

  const findWords = () => {
    const text = transientBoard;
    if (text && isValidBoard(text)) {
      // TODO: add validation message
      setUrlState({ ...urlState, board: text });
    }
  };

  const submitForm = (e: FormEvent) => {
    e.preventDefault();
    findWords();
  };

  const setWordlist = (newWordlist: string) => {
    setUrlState({ ...urlState, wordlist: newWordlist });
  };

  const setDims = (newDimsStr: string) => {
    const oldDims = urlState.dims;
    const newDims = Number(newDimsStr) as 33 | 44 | 55;
    const newBoard = adjustBoard(urlState.board, oldDims, newDims);
    setUrlState({ ...urlState, board: newBoard, dims: newDims });
  };

  const wordlistNetworkPath = WORDLISTS.find((wl) => wl.key === wordlist)!.path;

  return (
    <>
      <div id="boggle-app">
        <form onSubmit={submitForm}>
          <input
            type="text"
            width={20}
            value={transientBoard}
            onChange={(e) => setTransientBoard(e.currentTarget.value)}
          />
          <br />
          <select
            value={wordlist}
            onChange={(e) => setWordlist(e.currentTarget.value)}
          >
            {WORDLISTS.map((item) => (
              <option key={item.key} value={item.key}>
                {item.display}
              </option>
            ))}
          </select>
          {" "}
          <select value={dims} onChange={(e) => setDims(e.currentTarget.value)}>
            <option value={33}>3x3</option>
            <option value={44}>4x4</option>
            <option value={55}>5x5</option>
          </select>
          <br />
          <input
            id="multiboggle"
            type="checkbox"
            checked={multiboggle}
            onChange={(e) =>
              setUrlState({ ...urlState, multiboggle: e.currentTarget.checked })
            }
          />
          {" "}
          <label htmlFor="multiboggle">Allow Repeats ("multiboggle")</label>
          <br />
          <button onClick={findWords}>Find Words</button>
        </form>
        <React.Suspense fallback={<div>Loading...</div>}>
          {board ? (
            <BoggleUI
              wordlist={wordlistNetworkPath}
              board={board}
              multiboggle={multiboggle}
              dims={dims}
            />
          ) : null}
        </React.Suspense>
      </div>
    </>
  );
}

export default App;
