; ModuleID = 'MovForth'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  call void @print(i32 42)
  call void @print(i32 42)
  call void @print(i32 42)
  ret void

; uselistorder directives
  uselistorder i32 42, { 2, 0, 1 }
}

define private void @print(i32 %par0.1reg) {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 %par0.1reg)
  ret void
}
