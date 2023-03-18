(define
	(problem grounded-BLOCKS-4-0)
	(:domain grounded-BLOCKS)
	(:init
		(clear_d)
(handempty)
(ontable_a)
(ontable_b)
(clear_a)
(clear_c)
(ontable_d)
(clear_b)
(ontable_c)
	)
	(:goal
		(and
            (on_b_a)
(on_c_b)
(on_d_c)
        )
	)
)
