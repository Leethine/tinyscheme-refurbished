(define (! x)
  (if 
    (= x 0) "1"
    (big-mul (number->string x) (! (- x 1)))
  )
)