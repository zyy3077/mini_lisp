## Lv7+ 文档说明
1. 多行输入
   ```lisp
    >>>(define (f x)
            (+ x x))
    ()
    >>>(f 1)
    2
    ```
    实现：`(main.cpp)`直到tokens中括号匹配后开始parse
<hr>

2. 一次求值多个表达式
   ```lisp
    >>>1 2
    1
    2
    >>>(define x 5) (define y 10)
    ()
    ()
    >>>x y
    5
    10
    ```
    实现：`(main.cpp)splitExpression`函数
<hr>

3. 多行注释
    ```lisp
    >>>(define #|z 5 ) |# w 2)
    ()
    >>>w
    2
    >>>z
    Error: Variable "z" not defined.
    >>>(define x 5)
        #|
        这是一个多行注释。
        它可以跨越多行。
        |# 
        (define y 10)
    ()
    ()
    >>>x y
    5
    10
    ```
    实现：`(tokenizer.cpp)`多行注释部分

