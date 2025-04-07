import classNames from "classnames";

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

export interface BoggleLetterRowProps {
  row: number;
  letters: string[];
  arrows: [XY, XY][];
}

interface RowArrowClass {
  left?: boolean;
  right?: boolean;
}

function BoggleLetterRow(props: BoggleLetterRowProps) {
  const { row, letters, arrows } = props;
  const classes: RowArrowClass[] = Array(3)
    .fill(null)
    .map(() => ({}));
  for (const [a, b] of arrows) {
    const x = Math.min(a.x, b.x);
    classes[x][a.x > b.x ? "left" : "right"] = true;
  }
  return (
    <tr>
      <td>
        <input type="text" value={letters[0]} size={2} readOnly />
      </td>
      <td>
        <div className={classNames("arrow", classes[0])} />
      </td>
      <td>
        <input type="text" value={letters[1]} size={2} readOnly />
      </td>
      <td>
        <div className={classNames("arrow", classes[1])} />
      </td>
      <td>
        <input type="text" value={letters[2]} size={2} readOnly />
      </td>
      <td>
        <div className={classNames("arrow", classes[2])} />
      </td>
      <td>
        <input type="text" value={letters[3]} size={2} readOnly />
      </td>
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
    <tr>
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

  return (
    <table className="boggle-board">
      <tbody>
        <BoggleLetterRow row={0} letters={grid[0]} arrows={inRowArrows[0]} />
        <BoggleArrowRow rowAbove={0} arrows={betweenRowArrows[0]} />
        <BoggleLetterRow row={1} letters={grid[1]} arrows={inRowArrows[1]} />
        <BoggleArrowRow rowAbove={0} arrows={betweenRowArrows[1]} />
        <BoggleLetterRow row={2} letters={grid[2]} arrows={inRowArrows[2]} />
        <BoggleArrowRow rowAbove={0} arrows={betweenRowArrows[2]} />
        <BoggleLetterRow row={3} letters={grid[3]} arrows={inRowArrows[3]} />
      </tbody>
    </table>
  );
}
