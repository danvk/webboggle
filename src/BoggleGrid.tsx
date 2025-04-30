import classNames from "classnames";
import React from "react";
import { parseBoard, range, SCORES } from "./boggle";

interface XY {
  x: number;
  y: number;
}

function indexToCoord(index: number, numCols: number): XY {
  const y = Math.floor(index / numCols);
  const x = index % numCols;
  return { x, y };
}

function coordToIndex({ x, y }: XY, numCols: number) {
  return numCols * y + x;
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
  const classes: RowArrowClass[] = letters.map(() => ({}));
  for (const [a, b] of arrows) {
    const x = Math.min(a.x, b.x);
    classes[x][a.x > b.x ? "left" : "right"] = true;
  }
  const displayLetters = letters.map(boggleToUpper);
  const first = selectedPath?.[0];
  const last = selectedPath?.[selectedPath.length - 1];
  const numCols = letters.length;
  const indices = letters.map((_, x) => coordToIndex({ x, y: row }, numCols));
  return (
    <tr>
      {displayLetters.map((displayLetter, x) => (
        <React.Fragment key={x}>
          {x > 0 ? (
            <td>
              <div className={classNames("arrow", classes[x - 1])} />
            </td>
          ) : null}
          <td
            className={classNames({
              first: indices[x] === first,
              last: indices[x] === last,
              path: selectedPath?.includes(indices[x]),
            })}
          >
            <input type="text" value={displayLetter} size={2} readOnly />
          </td>
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
  numCols: number;
  arrows: [XY, XY][];
}

function BoggleArrowRow(props: BoggleArrowRowProps) {
  const { arrows, numCols } = props;
  const vertClasses: VerticalArrowClass[] = range(numCols).map(() => ({}));
  const diagClasses: DiagonalArrowClass[] = range(numCols - 1).map(() => ({}));

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
      {vertClasses.map((vertClass, i) => (
        <React.Fragment key={i}>
          {i > 0 ? (
            <td>
              <div className={classNames("arrow", diagClasses[i - 1])} />
            </td>
          ) : null}
          <td>
            <div className={classNames("arrow", vertClass)} />
          </td>
        </React.Fragment>
      ))}
    </tr>
  );
}

function getArrows(indices: number[], numCols: number) {
  const path = indices.map((i) => indexToCoord(i, numCols));
  const pathPairs = path.slice(1).map((b, i) => [path[i], b] as const);
  const inRowArrows: [XY, XY][][] = range(numCols).map(() => []);
  const betweenRowArrows: [XY, XY][][] = range(numCols - 1).map(() => []);
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
  dims: 33 | 44 | 55;
  selectedPath: number[] | null;
}

export function BoggleGrid(props: BoggleGridProps) {
  const { board, dims, selectedPath } = props;
  const numCols = dims % 10; // assume numRows is the same
  const grid = parseBoard(board, numCols);
  const { inRowArrows, betweenRowArrows } = getArrows(
    selectedPath ?? [],
    numCols
  );
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
          {range(numCols).map((row) => (
            <React.Fragment key={row}>
              {row ? (
                <BoggleArrowRow
                  rowAbove={row - 1}
                  numCols={numCols}
                  arrows={betweenRowArrows[row - 1]}
                />
              ) : null}
              <BoggleLetterRow
                row={row}
                letters={grid[row]}
                arrows={inRowArrows[row]}
                selectedPath={selectedPath}
              />
            </React.Fragment>
          ))}
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
