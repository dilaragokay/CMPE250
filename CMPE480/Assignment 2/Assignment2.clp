(defmethod float ((?s STRING))
   (float (string-to-field ?s)))

(deftemplate illness
(slot illness-no)
(slot illness-name)
(slot prob)
(slot is-positive)
(slot input-no))

(deftemplate final-illness
(slot illness-name)
(slot prob)
(slot input-no))

(deftemplate illness-cond-prob
(slot illness-no)
(slot illness-name)
(slot symptom-no)
(slot prob-yes)
(slot prob-no))

(deftemplate input
(slot input-no)
(slot answer)
(slot symptom-no)
(slot illness-no))

(open "ILLNESS.txt" data "r")
(bind ?data (readline data))
(bind ?index 1)
    (while (neq ?data EOF)
       	(bind ?end-index (str-index "," ?data))
       	(bind ?illness-name (sub-string 1 (-  ?end-index 1) ?data))
       	(bind ?data (sub-string (+ ?end-index 1) (+ (str-index ",999" ?data) 4) ?data))
       	(bind ?end-index (str-index "," ?data))
       	(bind ?illness-prob (float (sub-string 1 (- ?end-index 1) ?data)))
       	(bind ?data (sub-string (+ ?end-index 1) (+ (str-index ",999" ?data) 4) ?data))
        (loop-for-count (?cnt1 1 40) do
        (assert (illness
          (illness-name ?illness-name)
          (prob ?illness-prob)
          (is-positive "y")
          (input-no (str-cat ?cnt1))
          (illness-no ?index))))
        (loop-for-count (?cnt1 1 40) do
        (assert (illness
          (illness-name ?illness-name)
          (prob (- 1 ?illness-prob))
          (is-positive "n")
          (illness-no ?index)
          (input-no (str-cat ?cnt1)))))
        (loop-for-count (?cnt1 1 40) do
        (assert (final-illness
          (illness-name ?illness-name)
          (prob ?illness-prob)
          (input-no (str-cat ?cnt1)))))
       	(while (neq (sub-string 1 3 ?data) "999")
       		;;; symptom-no
       		(bind ?end-index (str-index "," ?data))
       		(bind ?symptom-no (sub-string 1 (- ?end-index 1) ?data))
       		(bind ?data (sub-string (+ ?end-index 1) (+ (str-index ",999" ?data) 4) ?data))
       		;;; P(E|H)
       		(bind ?end-index (str-index "," ?data))
       		(bind ?p-e-h (float (sub-string 1 (- ?end-index 1) ?data)))
       		(bind ?data (sub-string (+ ?end-index 1) (+ (str-index ",999" ?data) 4) ?data))
       		;;; P(E|not H)
       		(bind ?end-index (str-index "," ?data))
       		(bind ?p-e-not-h (float (sub-string 1 (- ?end-index 1) ?data)))
       		(bind ?data (sub-string (+ ?end-index 1) (+ (str-index ",999" ?data) 4) ?data))
       		(assert (illness-cond-prob
	       		(illness-name ?illness-name)
	       		(symptom-no ?symptom-no)
	       		(illness-no ?index)
    			(prob-yes ?p-e-h)
    			(prob-no ?p-e-not-h)))
       )
       (readline data)
       (bind ?data (readline data))
       (bind ?index (+ 1 ?index)))
(close data)

(open "INPUTS.txt" data "r")
(bind ?data (readline data))
    (while (neq ?data EOF)
       	(bind ?end-index (str-index "," ?data))
       	(bind ?input-no (sub-string 1 (-  ?end-index 1) ?data))
       	(bind ?start-index (+ ?end-index 1))
       	(bind ?i 1)
       	(while (neq ?i 69)
       		(bind ?answer (sub-string ?start-index ?start-index ?data))
       		(if (eq ?answer "y")
			   then
			    (loop-for-count (?cnt1 1 90) do
			        (assert (input (input-no (str-cat ?input-no)) (answer "y") (symptom-no (str-cat ?i)) (illness-no ?cnt1))))
			    (bind ?start-index (+ ?start-index 4))
			   else
			   	(loop-for-count (?cnt1 1 90) do
			        (assert (input (input-no (str-cat ?input-no)) (answer "n") (symptom-no (str-cat ?i)) (illness-no ?cnt1))))
			    (bind ?start-index (+ ?start-index 3)))
       		(bind ?i (+ ?i 1))
       )
       (readline data)
       (bind ?data (readline data)))
(close data)

;;; calculates a and b
(defrule prob-of-having-illness
  ?f3<-(input (input-no ?input-no) (answer ?answer) (symptom-no ?symptom-no) (illness-no ?illness-no))
  (illness-cond-prob (illness-name ?illness-name) (symptom-no ?symptom-no) (prob-yes ?prob-yes) (prob-no ?prob-no))
  ?f1<-(illness (illness-name ?illness-name) (prob ?prob1) (is-positive "y") (input-no ?input-no) (illness-no ?illness-no))
  ?f2<-(illness (illness-name ?illness-name) (prob ?prob2) (is-positive "n") (input-no ?input-no) (illness-no ?illness-no))
  =>
  (if (eq ?answer "y")
  	then
      (modify ?f1 (prob (* ?prob1 ?prob-yes)))
      (modify ?f2 (prob (* ?prob2 ?prob-no)))
  	else
      (modify ?f1 (prob (* ?prob1 (- 1 ?prob-yes))))
      (modify ?f2 (prob (* ?prob2 (- 1 ?prob-no))))
  )
  (retract ?f3)
)

(run)

;;; calculates a/a + b
(defrule prob-of-having-illness-2
?f2<-(illness (illness-name ?illness-name) (prob ?prob1) (is-positive "y") (input-no ?input-no)) ;;; a
?f3<-(illness (illness-name ?illness-name) (prob ?prob2) (is-positive "n") (input-no ?input-no)) ;;; b
?f1<-(final-illness (illness-name ?illness-name) (input-no ?input-no))
=>
(printout t ?illness-name " " ?input-no " " ?prob1 " " ?prob2 crlf)
(if (eq (+ ?prob1 ?prob2) 0.0)
  	then
      (modify ?f1 (prob 0))
  	else
      (modify ?f1 (prob (/ ?prob1 (+ ?prob1 ?prob2))))
  )
(retract ?f3 ?f2)
)

(run)

(open "OUTPUTS.txt" outputFile "w")

(defrule find-max-value
   (final-illness (illness-name ?illness-name) (prob ?prob1) (input-no ?input-no))
   (not (final-illness (prob ?prob2&:(> ?prob2 ?prob1)) (input-no ?input-no)))
   =>
   (printout outputFile ?input-no "," ?illness-name crlf))

(run)

(close outputFile)



