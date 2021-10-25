; ModuleID = 'MovForth'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  %reg1.1 = alloca i32
  br label %"1.br"

"1.br":                                           ; preds = %entry
  store i32 1, i32* %reg1.1
  %0 = load i32, i32* %reg1.1
  call void @conditional(i32 %0)
  ret void
}

define private void @conditional(i32 %par1.1reg) {
entry:
  %reg4.2 = alloca i32
  %reg4.1 = alloca i32
  %par1.1 = alloca i32
  br label %"1.br"

"1.br":                                           ; preds = %entry
  store i32 %par1.1reg, i32* %par1.1
  %0 = load i32, i32* %par1.1
  %1 = icmp eq i32 0, %0
  br i1 %1, label %"3.br", label %"2.br"

"2.br":                                           ; preds = %"1.br"
  call void @gen32(i32* %reg4.1, i32* %reg4.2)
  br label %"4.br"

"3.br":                                           ; preds = %"1.br"
  call void @gen42(i32* %reg4.1, i32* %reg4.2)
  br label %"4.br"

"4.br":                                           ; preds = %"3.br", %"2.br"
  %2 = load i32, i32* %reg4.2
  call void @print(i32 %2)
  %3 = load i32, i32* %reg4.1
  call void @print(i32 %3)
  ret void
}

define private void @gen32(i32* %ret1.1reg, i32* %ret1.2reg) {
entry:
  br label %"1.br"

"1.br":                                           ; preds = %entry
  store i32 32, i32* %ret1.1reg
  store i32 32, i32* %ret1.2reg
  ret void
}

define private void @gen42(i32* %ret1.1reg, i32* %ret1.2reg) {
entry:
  br label %"1.br"

"1.br":                                           ; preds = %entry
  store i32 42, i32* %ret1.1reg
  store i32 42, i32* %ret1.2reg
  ret void
}

define private void @print(i32 %par1.1reg) {
entry:
  %par1.1 = alloca i32
  br label %"1.br"

"1.br":                                           ; preds = %entry
  store i32 %par1.1reg, i32* %par1.1
  %0 = load i32, i32* %par1.1
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %0)
  ret void
}
