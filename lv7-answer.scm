(define (less x lst) 
        (filter (lambda (a) (> x a)) lst))
(define (greater x lst) 
        (filter (lambda (a) (<= x a)) lst))
        
(define (quicksort lst) 
        (if (null? lst) 
            '() 
            (append (quicksort (less (car lst) (cdr lst))) 
                    (list (car lst)) 
                    (quicksort (greater (car lst) (cdr lst))))))

(define lst '(12 71 2 15 29 82 87 8 18 66 81 25 63 97 40 3 93 58 53 31 47))
(displayln (quicksort lst))

(define (double x) (+ x x))
(define x 42)
(double x)
(print (double x))