; ModuleID = 'MovForth'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  br label %"2.br"

"2.br":                                           ; preds = %"2.br", %entry
  %reg2.1.0 = phi i32 [ 12, %entry ], [ %1, %"2.br" ]
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 %reg2.1.0)
  %1 = add i32 %reg2.1.0, -1
  %2 = icmp eq i32 %1, 0
  br i1 %2, label %"3.br", label %"2.br"

"3.br":                                           ; preds = %"2.br"
  ret void
}
