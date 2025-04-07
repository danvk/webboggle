function parseBoard(board: string) {
  return [
    board.slice(0, 4).split(""),
    board.slice(4, 8).split(""),
    board.slice(8, 12).split(""),
    board.slice(12, 16).split(""),
  ];
}

export interface BoggleLetterRowProps {
  row: number;
  letters: string[];
}

function BoggleLetterRow(props: BoggleLetterRowProps) {
  const { row, letters } = props;
  return (
    <tr>
      <td>
        <input type="text" value={letters[0]} size={2} />
        <input type="text" value={letters[1]} size={2} />
        <input type="text" value={letters[2]} size={2} />
        <input type="text" value={letters[3]} size={2} />
      </td>
    </tr>
  );
}

export interface BoggleGridProps {
  board: string;
  selectedPath: number[] | null;
}

export function BoggleGrid(props: BoggleGridProps) {
  const { board } = props;
  const grid = parseBoard(board);

  return (
    <table className="boggle-board">
      <tbody>
        <BoggleLetterRow row={0} letters={grid[0]} />
        <BoggleLetterRow row={1} letters={grid[1]} />
        <BoggleLetterRow row={2} letters={grid[2]} />
        <BoggleLetterRow row={3} letters={grid[3]} />
      </tbody>
    </table>
  );
}
