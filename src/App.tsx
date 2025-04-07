import { FormEvent, useRef, useState } from "react";
import { BoggleUI } from "./BoggleUI";
import React from "react";

import "./App.css";
import "./Boggle.css";

const WORDLISTS = [
  { display: "ENABLE2K", path: "/wordlists/enable2k.txt" },
  { display: "NASPA23", path: "/wordlists/naspa2023.txt" },
  { display: "OSPD5", path: "/wordlists/ospd5.txt" },
  { display: "SOWPODS", path: "/wordlists/sowpods.txt" },
  { display: "TWL06", path: "/wordlists/twl06.txt" },
  { display: "YAWL", path: "/wordlists/yawl.txt" },
];

function App() {
  const [board, setBoard] = useState<string | null>(null);
  const [wordlist, setWordlist] = useState<number>(0);

  const textBox = useRef<HTMLInputElement | null>(null);

  const findWords = () => {
    const text = textBox.current?.value;
    if (text && text.length == 16) {
      // TODO: qu; add validation message
      setBoard(text);
    }
  };
  const submitForm = (e: FormEvent) => {
    e.preventDefault();
    findWords();
  };

  return (
    <>
      <h1>Online Boggle Solver</h1>
      <div id="boggle-app">
        <form onSubmit={submitForm}>
          <input
            type="text"
            width={20}
            placeholder="abcdefghijklmnop"
            ref={textBox}
          />
          <br />
          <select
            value={wordlist}
            onChange={(e) => setWordlist(Number(e.currentTarget.value))}
          >
            {WORDLISTS.map((item, i) => (
              <option key={i} value={i}>
                {item.display}
              </option>
            ))}
          </select>
          <br />
          <button onClick={findWords}>Find Words</button>
        </form>
        <React.Suspense fallback={<div>Loading...</div>}>
          {board ? (
            <BoggleUI
              wordlist={WORDLISTS[wordlist].path}
              board={board}
              multiboggle={false}
            />
          ) : null}
        </React.Suspense>
      </div>
    </>
  );
}

export default App;
