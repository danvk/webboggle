import { useRef, useState } from "react";
import { BoggleUI } from "./BoggleUI";
import React from "react";

import "./App.css";
import "./Boggle.css";

function App() {
  const [board, setBoard] = useState<string | null>(null);

  const textBox = useRef<HTMLInputElement | null>(null);

  const findWords = () => {
    const text = textBox.current?.value;
    if (text && text.length == 16) {
      // TODO: qu; add validation message
      setBoard(text);
    }
  };

  return (
    <>
      <h1>Online Boggle Solver</h1>
      <div>
        <input
          type="text"
          width={20}
          placeholder="abcdefghijklmnop"
          ref={textBox}
        />
        <button onClick={findWords}>Find Words</button>
        <React.Suspense fallback={<div>Loading...</div>}>
          {board ? (
            <BoggleUI
              wordlist="/wordlists/enable2k.txt"
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
