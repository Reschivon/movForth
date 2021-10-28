; ModuleID = 'MovForth'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  call void @conditional(i32 1)
  ret void
}

define private void @conditional(i32 %par0.1reg) {
entry:
  %reg4.2 = alloca i32, align 4
  %reg4.1 = alloca i32, align 4
  %0 = icmp eq i32 %par0.1reg, 0
  br i1 %0, label %"3.br", label %"2.br"

"2.br":                                           ; preds = %entry
  call void @gen32(i32* nonnull %reg4.1, i32* nonnull %reg4.2)
  br label %"4.br"

"3.br":                                           ; preds = %entry
  call void @gen42(i32* nonnull %reg4.1, i32* nonnull %reg4.2)
  br label %"4.br"

"4.br":                                           ; preds = %"3.br", %"2.br"
  %1 = load i32, i32* %reg4.2, align 4
  call void @print(i32 %1, i32 %1)
  %2 = load i32, i32* %reg4.1, align 4
  call void @print(i32 %2, i32 %2)
  ret void

; uselistorder directives
  uselistorder i32 1, { 2, 1, 0 }
}

define private void @gen32(i32* %ret1.1reg, i32* %ret1.2reg) {
entry:
  store i32 32, i32* %ret1.1reg, align 4
  store i32 32, i32* %ret1.2reg, align 4
  ret void
}

define private void @gen42(i32* %ret1.1reg, i32* %ret1.2reg) {
entry:
  store i32 42, i32* %ret1.1reg, align 4
  store i32 42, i32* %ret1.2reg, align 4
  ret void
}

define private void @print(i32 %par0.1reg) {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 %par0.1reg)
  ret void
}
