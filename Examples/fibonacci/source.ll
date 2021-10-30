; ModuleID = 'MovForth'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  br label %"2.br"

"2.br":                                           ; preds = %"2.br", %entry
  %reg2.3.0 = phi i32 [ 1000000, %entry ], [ %2, %"2.br" ]
  %reg2.2.0 = phi i32 [ 1, %entry ], [ %0, %"2.br" ]
  %reg2.1.0 = phi i32 [ 1, %entry ], [ %reg2.2.0, %"2.br" ]
  %0 = add i32 %reg2.1.0, %reg2.2.0
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 %reg2.2.0)
  %2 = add i32 %reg2.3.0, -1
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %"3.br", label %"2.br"

"3.br":                                           ; preds = %"2.br"
  ret void

; uselistorder directives
  uselistorder i32 %reg2.2.0, { 2, 0, 1 }
}
