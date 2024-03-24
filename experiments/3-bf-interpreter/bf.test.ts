import dedent from "npm:dedent@1.5.1"
import { bf } from "./bf.ts"
import { assertEquals } from "https://deno.land/std@0.220.0/assert/mod.ts"

const input = (inputs: number[]) => {
  let i = 0
  return () => inputs.at(i++)
}

Deno.test("ABC", () => {
  const program = dedent`
    ++++++++++++++++++++++++++++++++++++++++
    +++++++++++++++++++++++++.+.+.>++++++++++.
  `
  const output = bf(program, input([]))

  assertEquals(output, "ABC\n")
})

Deno.test("Hello, World!", () => {
  const program = dedent`
    +++++++++[>++++++++>+++++++++++>+++>+<<<<-]>.>++.+++++++..+++.
    >+++++.<<+++++++++++++++.>.+++.------.--------.>+.>+.
  `
  const output = bf(program, input([]))

  assertEquals(output, "Hello World!\n")
})

Deno.test("input", () => {
  const program = dedent`
    ,.,.,.
  `
  const output = bf(program, input([65, 66, 67]))

  assertEquals(output, "ABC")
})
