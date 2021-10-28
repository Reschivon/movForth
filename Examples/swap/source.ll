; ModuleID = 'MovForth'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 1)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 2)
  ret void
}
