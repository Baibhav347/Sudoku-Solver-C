# Full-Stack Sudoku Solver (C + Web)

A high-performance Sudoku Solver using a C backend server and an HTML/JS frontend.

## Project Structure
- **Backend:** C (using Winsock for networking).
- **Frontend:** HTML, CSS, and JavaScript.

## How to Run
1. **Compile the Backend:**
   - Open `sudoko solver.c` in Code::Blocks or Dev-C++.
   - Ensure the linker settings include `-lws2_32` (for Winsock).
   - Compile and Run.
   - Keep the black terminal window OPEN.

2. **Launch the Frontend:**
   - Double-click `index.html`.
   - The interface will open in your web browser.

3. **Usage:**
   - Enter numbers into the grid.
   - Click "Solve".
   - The solution is fetched from the C server and displayed instantly.
