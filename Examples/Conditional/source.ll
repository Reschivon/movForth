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
  %0 = icmp eq i32 %par0.1reg, 0
  %reg4.1.0 = select i1 %0, i32 42, i32 32
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 %reg4.1.0)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 %reg4.1.0)
  ret void
}
