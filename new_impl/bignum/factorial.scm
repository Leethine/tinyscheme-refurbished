;; This doesn't work after some x
(define (fact x)
  (if 
    (= x 0) 1
    (* x (fact (- x 1)))
  )
)

;; This supports big numbers
(define (! x)
  (if 
    (= x 0) "1"
    (big-mul (number->string x) (! (- x 1)))
  )
)

(display (! 100))
(newline)