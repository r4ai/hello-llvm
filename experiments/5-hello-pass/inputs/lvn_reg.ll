define i32 @test_lvn(i32 %a, i32 %b) {
entry:
  %add1 = add i32 %a, %b
  %mul1 = mul i32 %add1, 2

  %add2 = add i32 %a, %b
  %add3 = add i32 %b, %a
  %mul2 = mul i32 %add2, 2

  %result1 = add i32 %mul1, %mul2
  %result2 = add i32 %result1, %add3

  ret i32 %result2
}

define i32 @test_comparison(i32 %x, i32 %y) {
entry:
  %cmp1 = icmp eq i32 %x, %y
  %cmp2 = icmp eq i32 %x, %y

  %z1 = zext i1 %cmp1 to i32
  %z2 = zext i1 %cmp2 to i32

  %result = add i32 %z1, %z2

  ret i32 %result
}
