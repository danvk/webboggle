import classNames from "classnames";
import React from "react";
import { SCORES } from "./boggle";

function parseBoard(board: string) {
  return [
    board.slice(0, 4).split(""),
    board.slice(4, 8).split(""),
    board.slice(8, 12).split(""),
    board.slice(12, 16).split(""),
  ];
}

interface XY {
  x: number;
  y: number;
}

function indexToCoord(index: number): XY {
  const y = index >> 2;
  const x = index % 4;
  return { x, y };
}

function coordToIndex({ x, y }: XY) {
  return 4 * y + x;
}

interface RowArrowClass {
  left?: boolean;
  right?: boolean;
}

function boggleToUpper(letter: string) {
  if (letter == "q" || letter == "Q") {
    return "Qu";
  }
  return letter.toUpperCase();
}

export interface BoggleLetterRowProps {
  row: number;
  letters: string[];
  arrows: [XY, XY][];
  selectedPath: number[] | null;
}
function BoggleLetterRow(props: BoggleLetterRowProps) {
  const { row, letters, arrows, selectedPath } = props;
  const classes: RowArrowClass[] = Array(3)
    .fill(null)
    .map(() => ({}));
  for (const [a, b] of arrows) {
    const x = Math.min(a.x, b.x);
    classes[x][a.x > b.x ? "left" : "right"] = true;
  }
  const displayLetters = letters.map(boggleToUpper);
  const first = selectedPath?.[0];
  const last = selectedPath?.[selectedPath.length - 1];
  const indices = [0, 1, 2, 3].map((x) => coordToIndex({ x, y: row }));
  return (
    <tr>
      {[0, 1, 2, 3].map((x) => (
        <React.Fragment key={x}>
          <td
            className={classNames({
              first: indices[x] === first,
              last: indices[x] === last,
              path: selectedPath?.includes(indices[x]),
            })}
          >
            <input type="text" value={displayLetters[x]} size={2} readOnly />
          </td>
          {x < 3 ? (
            <td>
              <div className={classNames("arrow", classes[x])} />
            </td>
          ) : null}
        </React.Fragment>
      ))}
    </tr>
  );
}

interface VerticalArrowClass {
  up?: boolean;
  down?: boolean;
}
interface DiagonalArrowClass extends RowArrowClass, VerticalArrowClass {}

interface BoggleArrowRowProps {
  rowAbove: number;
  arrows: [XY, XY][];
}

function BoggleArrowRow(props: BoggleArrowRowProps) {
  const { arrows } = props;
  const vertClasses: VerticalArrowClass[] = [{}, {}, {}, {}];
  const diagClasses: DiagonalArrowClass[] = [{}, {}, {}];
  for (const [a, b] of arrows) {
    const x = Math.min(a.x, b.x);
    if (a.x === b.x) {
      vertClasses[x][a.y < b.y ? "down" : "up"] = true;
    } else {
      diagClasses[x][a.x > b.x ? "left" : "right"] = true;
      diagClasses[x][a.y < b.y ? "down" : "up"] = true;
    }
  }
  return (
    <tr className="arrow-row">
      <td>
        <div className={classNames("arrow", vertClasses[0])} />
      </td>
      <td>
        <div className={classNames("arrow", diagClasses[0])} />
      </td>
      <td>
        <div className={classNames("arrow", vertClasses[1])} />
      </td>
      <td>
        <div className={classNames("arrow", diagClasses[1])} />
      </td>
      <td>
        <div className={classNames("arrow", vertClasses[2])} />
      </td>
      <td>
        <div className={classNames("arrow", diagClasses[2])} />
      </td>
      <td>
        <div className={classNames("arrow", vertClasses[3])} />
      </td>
    </tr>
  );
}

function getArrows(indices: number[]) {
  const path = indices.map(indexToCoord);
  const pathPairs = path.slice(1).map((b, i) => [path[i], b] as const);
  const inRowArrows: [XY, XY][][] = [[], [], [], []];
  const betweenRowArrows: [XY, XY][][] = [[], [], []];
  for (const [a, b] of pathPairs) {
    if (a.y == b.y) {
      inRowArrows[a.y].push([a, b]);
    } else {
      const y = Math.min(a.y, b.y);
      betweenRowArrows[y].push([a, b]);
    }
  }
  return { inRowArrows, betweenRowArrows };
}

export interface BoggleGridProps {
  board: string;
  selectedPath: number[] | null;
}

export function BoggleGrid(props: BoggleGridProps) {
  const { board, selectedPath } = props;
  const grid = parseBoard(board);
  const { inRowArrows, betweenRowArrows } = getArrows(selectedPath ?? []);
  const selectedWord =
    selectedPath &&
    selectedPath
      .map((i) => board[i])
      .join("")
      .replace(/q/g, "qu");

  return (
    <div className="boggle-board">
      <table>
        <tbody>
          <BoggleLetterRow
            row={0}
            letters={grid[0]}
            arrows={inRowArrows[0]}
            selectedPath={selectedPath}
          />
          <BoggleArrowRow rowAbove={0} arrows={betweenRowArrows[0]} />
          <BoggleLetterRow
            row={1}
            letters={grid[1]}
            arrows={inRowArrows[1]}
            selectedPath={selectedPath}
          />
          <BoggleArrowRow rowAbove={0} arrows={betweenRowArrows[1]} />
          <BoggleLetterRow
            row={2}
            letters={grid[2]}
            arrows={inRowArrows[2]}
            selectedPath={selectedPath}
          />
          <BoggleArrowRow rowAbove={0} arrows={betweenRowArrows[2]} />
          <BoggleLetterRow
            row={3}
            letters={grid[3]}
            arrows={inRowArrows[3]}
            selectedPath={selectedPath}
          />
        </tbody>
      </table>
      {selectedWord ? (
        <div className="selected-word">
          <strong>{selectedWord}</strong>: {SCORES[selectedWord.length]} point
          {SCORES[selectedWord.length] > 1 ? "s" : ""}
        </div>
      ) : (
        <div className="selected-word">{" "}</div>
      )}
    </div>
  );
}
