export const bf = (
  program: string,
  input: () => unknown = () => prompt("Input"),
) => {
  const memory: number[] = new Array(30000).fill(0)
  let pointer = 0

  let instructionPointer = 0
  let output = ""
  while (instructionPointer < program.length) {
    const instruction = program[instructionPointer]
    switch (instruction) {
      case ">":
        pointer++
        break
      case "<":
        pointer--
        break
      case "+":
        memory[pointer] = (memory[pointer] || 0) + 1
        break
      case "-":
        memory[pointer] = (memory[pointer] || 0) - 1
        break
      case ".":
        output += String.fromCharCode(memory[pointer])
        break
      case ",":
        memory[pointer] = Number(input() ?? 0)
        break
      case "[":
        if (!memory[pointer]) {
          let bracketCount = 1
          while (bracketCount) {
            instructionPointer++
            if (program[instructionPointer] === "[") {
              bracketCount++
            } else if (program[instructionPointer] === "]") {
              bracketCount--
            }
          }
        }
        break
      case "]":
        if (memory[pointer]) {
          let bracketCount = 1
          while (bracketCount) {
            instructionPointer--
            if (program[instructionPointer] === "]") {
              bracketCount++
            } else if (program[instructionPointer] === "[") {
              bracketCount--
            }
          }
        }
        break
    }

    instructionPointer++
  }
  return output
}

if (import.meta.main) {
  const program = Deno.args[0]
  if (!program) {
    console.error("No program provided!")
    console.error("Usage: deno run bf.ts <program>")
    Deno.exit(1)
  }

  const output = bf(program)

  console.log(output)
}
