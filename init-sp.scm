;; Standard procedures

(define (memq obj list)
  (cond
    ((null? list) #f)
    ((eq? obj (car list)) list)
    (else (memq obj (cdr list)))
  )
)

(define (memv obj list)
  (cond
    ((null? list) #f)
    ((eqv? obj (car list)) list)
    (else (memq obj (cdr list)))
  )
)

(define (member obj list)
  (cond
    ((null? list) #f)
    ((equal? obj (car list)) list)
    (else (memq obj (cdr list)))
  )
)

