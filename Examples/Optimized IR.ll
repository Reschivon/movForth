; ModuleID = '../MovForth.ll'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #0

; Function Attrs: nounwind
define void @main() local_unnamed_addr #0 {
entry:
  %0 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 32) #0
  %1 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 32) #0
  ret void
}

attributes #0 = { nounwind }
