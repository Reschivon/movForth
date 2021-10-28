; ModuleID = 'MovForth'
source_filename = "MovForth"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  %reg1.66 = alloca i32, align 4
  %reg1.65 = alloca i32, align 4
  %reg1.64 = alloca i32, align 4
  %reg1.63 = alloca i32, align 4
  %reg1.62 = alloca i32, align 4
  %reg1.61 = alloca i32, align 4
  %reg1.60 = alloca i32, align 4
  %reg1.59 = alloca i32, align 4
  %reg1.58 = alloca i32, align 4
  %reg1.57 = alloca i32, align 4
  %reg1.56 = alloca i32, align 4
  %reg1.55 = alloca i32, align 4
  %reg1.54 = alloca i32, align 4
  %reg1.53 = alloca i32, align 4
  %reg1.52 = alloca i32, align 4
  %reg1.51 = alloca i32, align 4
  %reg1.50 = alloca i32, align 4
  %reg1.49 = alloca i32, align 4
  %reg1.48 = alloca i32, align 4
  %reg1.47 = alloca i32, align 4
  %reg1.46 = alloca i32, align 4
  %reg1.45 = alloca i32, align 4
  %reg1.44 = alloca i32, align 4
  %reg1.43 = alloca i32, align 4
  %reg1.42 = alloca i32, align 4
  %reg1.41 = alloca i32, align 4
  %reg1.40 = alloca i32, align 4
  %reg1.39 = alloca i32, align 4
  %reg1.38 = alloca i32, align 4
  %reg1.37 = alloca i32, align 4
  %reg1.36 = alloca i32, align 4
  %reg1.35 = alloca i32, align 4
  %reg1.34 = alloca i32, align 4
  %reg1.33 = alloca i32, align 4
  %reg1.32 = alloca i32, align 4
  %reg1.31 = alloca i32, align 4
  %reg1.30 = alloca i32, align 4
  %reg1.29 = alloca i32, align 4
  %reg1.28 = alloca i32, align 4
  %reg1.27 = alloca i32, align 4
  %reg1.26 = alloca i32, align 4
  %reg1.25 = alloca i32, align 4
  %reg1.24 = alloca i32, align 4
  %reg1.23 = alloca i32, align 4
  %reg1.22 = alloca i32, align 4
  %reg1.21 = alloca i32, align 4
  %reg1.20 = alloca i32, align 4
  %reg1.19 = alloca i32, align 4
  %reg1.18 = alloca i32, align 4
  %reg1.17 = alloca i32, align 4
  %reg1.16 = alloca i32, align 4
  %reg1.15 = alloca i32, align 4
  %reg1.14 = alloca i32, align 4
  %reg1.13 = alloca i32, align 4
  %reg1.12 = alloca i32, align 4
  %reg1.11 = alloca i32, align 4
  %reg1.10 = alloca i32, align 4
  %reg1.9 = alloca i32, align 4
  %reg1.8 = alloca i32, align 4
  %reg1.7 = alloca i32, align 4
  %reg1.6 = alloca i32, align 4
  %reg1.5 = alloca i32, align 4
  %reg1.4 = alloca i32, align 4
  %reg1.3 = alloca i32, align 4
  call void @fib(i32 1, i32 1, i32* nonnull %reg1.3, i32* nonnull %reg1.4)
  %0 = load i32, i32* %reg1.4, align 4
  %1 = load i32, i32* %reg1.3, align 4
  call void @fib(i32 %0, i32 %1, i32* nonnull %reg1.5, i32* nonnull %reg1.6)
  %2 = load i32, i32* %reg1.6, align 4
  %3 = load i32, i32* %reg1.5, align 4
  call void @fib(i32 %2, i32 %3, i32* nonnull %reg1.7, i32* nonnull %reg1.8)
  %4 = load i32, i32* %reg1.8, align 4
  %5 = load i32, i32* %reg1.7, align 4
  call void @fib(i32 %4, i32 %5, i32* nonnull %reg1.9, i32* nonnull %reg1.10)
  %6 = load i32, i32* %reg1.10, align 4
  %7 = load i32, i32* %reg1.9, align 4
  call void @fib(i32 %6, i32 %7, i32* nonnull %reg1.11, i32* nonnull %reg1.12)
  %8 = load i32, i32* %reg1.12, align 4
  %9 = load i32, i32* %reg1.11, align 4
  call void @fib(i32 %8, i32 %9, i32* nonnull %reg1.13, i32* nonnull %reg1.14)
  %10 = load i32, i32* %reg1.14, align 4
  %11 = load i32, i32* %reg1.13, align 4
  call void @fib(i32 %10, i32 %11, i32* nonnull %reg1.15, i32* nonnull %reg1.16)
  %12 = load i32, i32* %reg1.16, align 4
  %13 = load i32, i32* %reg1.15, align 4
  call void @fib(i32 %12, i32 %13, i32* nonnull %reg1.17, i32* nonnull %reg1.18)
  %14 = load i32, i32* %reg1.18, align 4
  %15 = load i32, i32* %reg1.17, align 4
  call void @fib(i32 %14, i32 %15, i32* nonnull %reg1.19, i32* nonnull %reg1.20)
  %16 = load i32, i32* %reg1.20, align 4
  %17 = load i32, i32* %reg1.19, align 4
  call void @fib(i32 %16, i32 %17, i32* nonnull %reg1.21, i32* nonnull %reg1.22)
  %18 = load i32, i32* %reg1.22, align 4
  %19 = load i32, i32* %reg1.21, align 4
  call void @fib(i32 %18, i32 %19, i32* nonnull %reg1.23, i32* nonnull %reg1.24)
  %20 = load i32, i32* %reg1.24, align 4
  %21 = load i32, i32* %reg1.23, align 4
  call void @fib(i32 %20, i32 %21, i32* nonnull %reg1.25, i32* nonnull %reg1.26)
  %22 = load i32, i32* %reg1.26, align 4
  %23 = load i32, i32* %reg1.25, align 4
  call void @fib(i32 %22, i32 %23, i32* nonnull %reg1.27, i32* nonnull %reg1.28)
  %24 = load i32, i32* %reg1.28, align 4
  %25 = load i32, i32* %reg1.27, align 4
  call void @fib(i32 %24, i32 %25, i32* nonnull %reg1.29, i32* nonnull %reg1.30)
  %26 = load i32, i32* %reg1.30, align 4
  %27 = load i32, i32* %reg1.29, align 4
  call void @fib(i32 %26, i32 %27, i32* nonnull %reg1.31, i32* nonnull %reg1.32)
  %28 = load i32, i32* %reg1.32, align 4
  %29 = load i32, i32* %reg1.31, align 4
  call void @fib(i32 %28, i32 %29, i32* nonnull %reg1.33, i32* nonnull %reg1.34)
  %30 = load i32, i32* %reg1.34, align 4
  %31 = load i32, i32* %reg1.33, align 4
  call void @fib(i32 %30, i32 %31, i32* nonnull %reg1.35, i32* nonnull %reg1.36)
  %32 = load i32, i32* %reg1.36, align 4
  %33 = load i32, i32* %reg1.35, align 4
  call void @fib(i32 %32, i32 %33, i32* nonnull %reg1.37, i32* nonnull %reg1.38)
  %34 = load i32, i32* %reg1.38, align 4
  %35 = load i32, i32* %reg1.37, align 4
  call void @fib(i32 %34, i32 %35, i32* nonnull %reg1.39, i32* nonnull %reg1.40)
  %36 = load i32, i32* %reg1.40, align 4
  %37 = load i32, i32* %reg1.39, align 4
  call void @fib(i32 %36, i32 %37, i32* nonnull %reg1.41, i32* nonnull %reg1.42)
  %38 = load i32, i32* %reg1.42, align 4
  %39 = load i32, i32* %reg1.41, align 4
  call void @fib(i32 %38, i32 %39, i32* nonnull %reg1.43, i32* nonnull %reg1.44)
  %40 = load i32, i32* %reg1.44, align 4
  %41 = load i32, i32* %reg1.43, align 4
  call void @fib(i32 %40, i32 %41, i32* nonnull %reg1.45, i32* nonnull %reg1.46)
  %42 = load i32, i32* %reg1.46, align 4
  %43 = load i32, i32* %reg1.45, align 4
  call void @fib(i32 %42, i32 %43, i32* nonnull %reg1.47, i32* nonnull %reg1.48)
  %44 = load i32, i32* %reg1.48, align 4
  %45 = load i32, i32* %reg1.47, align 4
  call void @fib(i32 %44, i32 %45, i32* nonnull %reg1.49, i32* nonnull %reg1.50)
  %46 = load i32, i32* %reg1.50, align 4
  %47 = load i32, i32* %reg1.49, align 4
  call void @fib(i32 %46, i32 %47, i32* nonnull %reg1.51, i32* nonnull %reg1.52)
  %48 = load i32, i32* %reg1.52, align 4
  %49 = load i32, i32* %reg1.51, align 4
  call void @fib(i32 %48, i32 %49, i32* nonnull %reg1.53, i32* nonnull %reg1.54)
  %50 = load i32, i32* %reg1.54, align 4
  %51 = load i32, i32* %reg1.53, align 4
  call void @fib(i32 %50, i32 %51, i32* nonnull %reg1.55, i32* nonnull %reg1.56)
  %52 = load i32, i32* %reg1.56, align 4
  %53 = load i32, i32* %reg1.55, align 4
  call void @fib(i32 %52, i32 %53, i32* nonnull %reg1.57, i32* nonnull %reg1.58)
  %54 = load i32, i32* %reg1.58, align 4
  %55 = load i32, i32* %reg1.57, align 4
  call void @fib(i32 %54, i32 %55, i32* nonnull %reg1.59, i32* nonnull %reg1.60)
  %56 = load i32, i32* %reg1.60, align 4
  %57 = load i32, i32* %reg1.59, align 4
  call void @fib(i32 %56, i32 %57, i32* nonnull %reg1.61, i32* nonnull %reg1.62)
  %58 = load i32, i32* %reg1.62, align 4
  %59 = load i32, i32* %reg1.61, align 4
  call void @fib(i32 %58, i32 %59, i32* nonnull %reg1.63, i32* nonnull %reg1.64)
  %60 = load i32, i32* %reg1.64, align 4
  %61 = load i32, i32* %reg1.63, align 4
  call void @fib(i32 %60, i32 %61, i32* nonnull %reg1.65, i32* nonnull %reg1.66)
  ret void

; uselistorder directives
  uselistorder i32 1, { 0, 1, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2 }
}

define private void @fib(i32 %par0.1reg, i32 %par0.2reg, i32* %ret0.1reg, i32* %ret1.1reg) {
entry:
  store i32 %par0.1reg, i32* %ret0.1reg, align 4
  %0 = add i32 %par0.2reg, %par0.1reg
  store i32 %0, i32* %ret1.1reg, align 4
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i64 0, i64 0), i32 %0)
  ret void

; uselistorder directives
  uselistorder i32 %0, { 1, 0 }
}
