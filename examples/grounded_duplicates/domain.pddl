(define
	(domain grounded-BLOCKS)
	(:requirements :strips)
	(:predicates
		(on_c_d)
(handempty)
(on_d_a)
(ontable_a)
(on_b_c)
(on_c_b)
(on_a_d)
(on_a_c)
(holding_c)
(holding_d)
(on_d_b)
(ontable_b)
(on_b_a)
(on_a_b)
(holding_b)
(clear_d)
(clear_b)
(ontable_c)
(on_c_a)
(on_d_c)
(on_b_d)
(clear_a)
(clear_c)
(holding_a)
(ontable_d)
	)
    
(:action stack_c_a#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#5
    :parameters ()
    :precondition (and (holding_c))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_b#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#5
    :parameters ()
    :precondition (and (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_d#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#5
    :parameters ()
    :precondition (and (holding_c))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_a_c#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#11
    :parameters ()
    :precondition (and (holding_a))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_b#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#11
    :parameters ()
    :precondition (and (holding_a))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_d#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#11
    :parameters ()
    :precondition (and (holding_a))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_b_c#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#6
    :parameters ()
    :precondition (and (holding_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_a#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#6
    :parameters ()
    :precondition (and (holding_b))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_d#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#6
    :parameters ()
    :precondition (and (holding_b))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_d_c#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#9
    :parameters ()
    :precondition (and (holding_d))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#12
    :parameters ()
    :precondition (and (on_b_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#14
    :parameters ()
    :precondition (and (on_d_b))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#16
    :parameters ()
    :precondition (and (on_a_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_a#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#9
    :parameters ()
    :precondition (and (holding_d))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#12
    :parameters ()
    :precondition (and (on_b_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#14
    :parameters ()
    :precondition (and (on_d_b))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#16
    :parameters ()
    :precondition (and (on_a_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_b#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#9
    :parameters ()
    :precondition (and (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#12
    :parameters ()
    :precondition (and (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#14
    :parameters ()
    :precondition (and (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#16
    :parameters ()
    :precondition (and (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action putdown_c#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#5
    :parameters ()
    :precondition (and (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_a#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#11
    :parameters ()
    :precondition (and (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_b#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#6
    :parameters ()
    :precondition (and (holding_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#10
    :parameters ()
    :precondition (and (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#13
    :parameters ()
    :precondition (and (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#15
    :parameters ()
    :precondition (and (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_d#2
    :parameters ()
    :precondition (and (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#4
    :parameters ()
    :precondition (and (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#8
    :parameters ()
    :precondition (and (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#9
    :parameters ()
    :precondition (and (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#12
    :parameters ()
    :precondition (and (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#14
    :parameters ()
    :precondition (and (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#16
    :parameters ()
    :precondition (and (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action stack_c_a#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_b#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_d#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_b_c#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_c#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_b_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_c)))
)


(:action stack_b_a#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_a#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (on_b_a) (clear_d) (handempty) (ontable_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_a)))
)


(:action stack_b_d#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_b_d#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (handempty) (on_b_d) (ontable_a) (clear_c) (clear_a) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)) (not (clear_d)))
)


(:action stack_d_c#18
    :parameters ()
    :precondition (and (on_a_b) (holding_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#20
    :parameters ()
    :precondition (and (on_c_a) (holding_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#22
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#24
    :parameters ()
    :precondition (and (holding_a) (handempty))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#26
    :parameters ()
    :precondition (and (holding_a) (ontable_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#28
    :parameters ()
    :precondition (and (holding_a) (on_b_d))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#30
    :parameters ()
    :precondition (and (holding_a) (on_c_b))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#32
    :parameters ()
    :precondition (and (holding_a) (on_a_d))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#34
    :parameters ()
    :precondition (and (holding_a) (on_a_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_a#18
    :parameters ()
    :precondition (and (on_a_b) (holding_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#20
    :parameters ()
    :precondition (and (on_c_a) (holding_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#22
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#24
    :parameters ()
    :precondition (and (holding_a) (handempty))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#26
    :parameters ()
    :precondition (and (holding_a) (ontable_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#28
    :parameters ()
    :precondition (and (holding_a) (on_b_d))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#30
    :parameters ()
    :precondition (and (holding_a) (on_c_b))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#32
    :parameters ()
    :precondition (and (holding_a) (on_a_d))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#34
    :parameters ()
    :precondition (and (holding_a) (on_a_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_b#18
    :parameters ()
    :precondition (and (on_a_b) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#20
    :parameters ()
    :precondition (and (on_c_a) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#22
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#24
    :parameters ()
    :precondition (and (holding_a) (handempty))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#26
    :parameters ()
    :precondition (and (holding_a) (ontable_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#28
    :parameters ()
    :precondition (and (holding_a) (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#30
    :parameters ()
    :precondition (and (holding_a) (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#32
    :parameters ()
    :precondition (and (holding_a) (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#34
    :parameters ()
    :precondition (and (holding_a) (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action putdown_c#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_b#17
    :parameters ()
    :precondition (and (on_b_a) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#19
    :parameters ()
    :precondition (and (holding_a) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#21
    :parameters ()
    :precondition (and (holding_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#23
    :parameters ()
    :precondition (and (on_d_c) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#25
    :parameters ()
    :precondition (and (holding_a) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#27
    :parameters ()
    :precondition (and (holding_a) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#29
    :parameters ()
    :precondition (and (holding_a) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#31
    :parameters ()
    :precondition (and (holding_a) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_b#33
    :parameters ()
    :precondition (and (holding_a) (clear_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_b)))
)


(:action putdown_d#18
    :parameters ()
    :precondition (and (on_a_b) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#20
    :parameters ()
    :precondition (and (on_c_a) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#22
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#24
    :parameters ()
    :precondition (and (holding_a) (handempty))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#26
    :parameters ()
    :precondition (and (holding_a) (ontable_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#28
    :parameters ()
    :precondition (and (holding_a) (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#30
    :parameters ()
    :precondition (and (holding_a) (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#32
    :parameters ()
    :precondition (and (holding_a) (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#34
    :parameters ()
    :precondition (and (holding_a) (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action pickup_c#8
    :parameters ()
    :precondition (and (handempty))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_a#8
    :parameters ()
    :precondition (and (handempty))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_b#8
    :parameters ()
    :precondition (and (handempty))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_d#8
    :parameters ()
    :precondition (and (handempty))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action stack_c_a#35
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#37
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#39
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#41
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#43
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#45
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#47
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#49
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_a#51
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (on_c_a) (clear_d) (handempty) (ontable_a) (ontable_b) (clear_c) (ontable_d) (clear_b)
(not (clear_a)) (not (holding_c)))
)


(:action stack_c_b#35
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#37
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#39
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#41
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#43
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#45
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#47
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#49
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_b#51
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (on_c_b) (clear_a) (clear_c) (ontable_d)
(not (clear_b)) (not (holding_c)))
)


(:action stack_c_d#35
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#37
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#39
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#41
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#43
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#45
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#47
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#49
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_c_d#51
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (on_c_d) (handempty) (ontable_a) (ontable_b) (clear_c) (clear_a) (ontable_d) (clear_b)
(not (clear_d)) (not (holding_c)))
)


(:action stack_a_c#17
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#18
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#19
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#20
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#21
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#22
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#23
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#24
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#25
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#26
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#27
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#28
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#29
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#30
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#31
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#32
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#33
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_c#34
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (clear_d) (handempty) (ontable_b) (clear_a) (on_a_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)) (not (clear_c)))
)


(:action stack_a_b#17
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#18
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#19
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#20
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#21
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#22
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#23
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#24
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#25
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#26
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#27
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#28
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#29
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#30
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#31
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#32
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#33
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_b#34
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (on_a_b) (clear_d) (handempty) (ontable_b) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (holding_a)) (not (clear_b)))
)


(:action stack_a_d#17
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#18
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#19
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#20
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#21
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#22
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#23
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#24
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#25
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#26
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#27
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#28
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#29
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#30
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#31
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#32
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#33
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_a_d#34
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (handempty) (ontable_b) (clear_c) (clear_a) (on_a_d) (ontable_d) (clear_b) (ontable_c)
(not (clear_d)) (not (holding_a)))
)


(:action stack_d_c#36
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#38
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#40
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#42
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#44
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#46
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#48
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#50
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_a#36
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#38
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#40
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#42
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#44
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#46
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#48
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#50
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_b#36
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#38
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#40
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#42
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#44
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#46
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#48
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#50
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action putdown_c#35
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#37
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#39
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#41
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#43
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#45
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#47
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#49
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_c#51
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_c)))
)


(:action putdown_a#17
    :parameters ()
    :precondition (and (holding_b) (on_b_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#18
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#19
    :parameters ()
    :precondition (and (holding_b) (on_c_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#20
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#21
    :parameters ()
    :precondition (and (holding_b) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#22
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#23
    :parameters ()
    :precondition (and (holding_b) (handempty))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#24
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#25
    :parameters ()
    :precondition (and (holding_b) (holding_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#26
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#27
    :parameters ()
    :precondition (and (holding_b) (holding_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#28
    :parameters ()
    :precondition (and (holding_b) (on_b_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#29
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#30
    :parameters ()
    :precondition (and (holding_b) (on_d_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#31
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#32
    :parameters ()
    :precondition (and (holding_b) (on_a_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#33
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_a#34
    :parameters ()
    :precondition (and (holding_b) (clear_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_a)))
)


(:action putdown_d#36
    :parameters ()
    :precondition (and (holding_b) (on_a_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#38
    :parameters ()
    :precondition (and (holding_b) (on_c_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#40
    :parameters ()
    :precondition (and (holding_b) (on_d_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#42
    :parameters ()
    :precondition (and (holding_b) (on_d_a))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#44
    :parameters ()
    :precondition (and (holding_b) (on_b_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#46
    :parameters ()
    :precondition (and (holding_b) (on_c_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#48
    :parameters ()
    :precondition (and (holding_b) (on_a_d))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#50
    :parameters ()
    :precondition (and (holding_b) (ontable_b))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action pickup_c#43
    :parameters ()
    :precondition (and (handempty) (ontable_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#61
    :parameters ()
    :precondition (and (handempty) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_a#43
    :parameters ()
    :precondition (and (handempty) (ontable_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#61
    :parameters ()
    :precondition (and (handempty) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_b#43
    :parameters ()
    :precondition (and (handempty) (ontable_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#61
    :parameters ()
    :precondition (and (handempty) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_d#43
    :parameters ()
    :precondition (and (handempty) (ontable_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#61
    :parameters ()
    :precondition (and (handempty) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action stack_d_c#52
    :parameters ()
    :precondition (and (on_b_a) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#54
    :parameters ()
    :precondition (and (on_c_d) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#56
    :parameters ()
    :precondition (and (on_d_c) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#58
    :parameters ()
    :precondition (and (on_d_a) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#60
    :parameters ()
    :precondition (and (on_b_d) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#62
    :parameters ()
    :precondition (and (on_c_b) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#64
    :parameters ()
    :precondition (and (on_a_d) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_c#66
    :parameters ()
    :precondition (and (on_a_c) (holding_c))
    :effect (and (on_d_c) (clear_d) (handempty) (ontable_a) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_c)))
)


(:action stack_d_a#52
    :parameters ()
    :precondition (and (on_b_a) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#54
    :parameters ()
    :precondition (and (on_c_d) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#56
    :parameters ()
    :precondition (and (on_d_c) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#58
    :parameters ()
    :precondition (and (on_d_a) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#60
    :parameters ()
    :precondition (and (on_b_d) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#62
    :parameters ()
    :precondition (and (on_c_b) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#64
    :parameters ()
    :precondition (and (on_a_d) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_a#66
    :parameters ()
    :precondition (and (on_a_c) (holding_c))
    :effect (and (clear_d) (handempty) (on_d_a) (ontable_a) (clear_c) (ontable_b) (clear_b) (ontable_c)
(not (holding_d)) (not (clear_a)))
)


(:action stack_d_b#52
    :parameters ()
    :precondition (and (on_b_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#54
    :parameters ()
    :precondition (and (on_c_d) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#56
    :parameters ()
    :precondition (and (on_d_c) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#58
    :parameters ()
    :precondition (and (on_d_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#60
    :parameters ()
    :precondition (and (on_b_d) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#62
    :parameters ()
    :precondition (and (on_c_b) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#64
    :parameters ()
    :precondition (and (on_a_d) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action stack_d_b#66
    :parameters ()
    :precondition (and (on_a_c) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (on_d_b) (clear_a) (clear_c) (ontable_b) (ontable_c)
(not (holding_d)) (not (clear_b)))
)


(:action putdown_d#52
    :parameters ()
    :precondition (and (on_b_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#54
    :parameters ()
    :precondition (and (on_c_d) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#56
    :parameters ()
    :precondition (and (on_d_c) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#58
    :parameters ()
    :precondition (and (on_d_a) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#60
    :parameters ()
    :precondition (and (on_b_d) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#62
    :parameters ()
    :precondition (and (on_c_b) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#64
    :parameters ()
    :precondition (and (on_a_d) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action putdown_d#66
    :parameters ()
    :precondition (and (on_a_c) (holding_c))
    :effect (and (clear_d) (handempty) (ontable_a) (ontable_b) (clear_a) (clear_c) (ontable_d) (clear_b) (ontable_c)
(not (holding_d)))
)


(:action pickup_c#97
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#115
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#133
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#151
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_a#97
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#115
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#133
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#151
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_b#97
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#115
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#133
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#151
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_d#97
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#115
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#133
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#151
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_c#187
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (clear_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#188
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (clear_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#198
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (clear_c) (ontable_d))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#205
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#206
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#216
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (ontable_d) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#241
    :parameters ()
    :precondition (and (clear_c) (ontable_a) (clear_b) (clear_d))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#242
    :parameters ()
    :precondition (and (clear_c) (handempty) (ontable_a) (clear_b))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#252
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (ontable_a) (clear_b))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#259
    :parameters ()
    :precondition (and (clear_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#260
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#270
    :parameters ()
    :precondition (and (ontable_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#295
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_c) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#296
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_c) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#306
    :parameters ()
    :precondition (and (ontable_b) (clear_c) (clear_a) (ontable_d))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#313
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_a) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#314
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#324
    :parameters ()
    :precondition (and (ontable_b) (clear_a) (ontable_d) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#349
    :parameters ()
    :precondition (and (clear_c) (clear_d) (clear_b) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#350
    :parameters ()
    :precondition (and (clear_c) (handempty) (clear_b) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#360
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (clear_b) (clear_a))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#367
    :parameters ()
    :precondition (and (clear_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#368
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_c#378
    :parameters ()
    :precondition (and (ontable_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_c) (clear_d) (ontable_a) (ontable_b) (clear_a) (ontable_d) (clear_b)
(not (handempty)) (not (clear_c)) (not (ontable_c)))
)


(:action pickup_a#187
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (clear_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#188
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (clear_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#198
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (clear_c) (ontable_d))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#205
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#206
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#216
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (ontable_d) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#241
    :parameters ()
    :precondition (and (clear_c) (ontable_a) (clear_b) (clear_d))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#242
    :parameters ()
    :precondition (and (clear_c) (handempty) (ontable_a) (clear_b))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#252
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (ontable_a) (clear_b))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#259
    :parameters ()
    :precondition (and (clear_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#260
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#270
    :parameters ()
    :precondition (and (ontable_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#295
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_c) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#296
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_c) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#306
    :parameters ()
    :precondition (and (ontable_b) (clear_c) (clear_a) (ontable_d))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#313
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_a) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#314
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#324
    :parameters ()
    :precondition (and (ontable_b) (clear_a) (ontable_d) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#349
    :parameters ()
    :precondition (and (clear_c) (clear_d) (clear_b) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#350
    :parameters ()
    :precondition (and (clear_c) (handempty) (clear_b) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#360
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (clear_b) (clear_a))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#367
    :parameters ()
    :precondition (and (clear_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#368
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_a#378
    :parameters ()
    :precondition (and (ontable_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (clear_d) (clear_c) (ontable_d) (holding_a) (ontable_b) (clear_b) (ontable_c)
(not (handempty)) (not (ontable_a)) (not (clear_a)))
)


(:action pickup_b#187
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (clear_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#188
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (clear_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#198
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (clear_c) (ontable_d))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#205
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#206
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#216
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (ontable_d) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#241
    :parameters ()
    :precondition (and (clear_c) (ontable_a) (clear_b) (clear_d))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#242
    :parameters ()
    :precondition (and (clear_c) (handempty) (ontable_a) (clear_b))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#252
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (ontable_a) (clear_b))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#259
    :parameters ()
    :precondition (and (clear_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#260
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#270
    :parameters ()
    :precondition (and (ontable_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#295
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_c) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#296
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_c) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#306
    :parameters ()
    :precondition (and (ontable_b) (clear_c) (clear_a) (ontable_d))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#313
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_a) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#314
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#324
    :parameters ()
    :precondition (and (ontable_b) (clear_a) (ontable_d) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#349
    :parameters ()
    :precondition (and (clear_c) (clear_d) (clear_b) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#350
    :parameters ()
    :precondition (and (clear_c) (handempty) (clear_b) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#360
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (clear_b) (clear_a))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#367
    :parameters ()
    :precondition (and (clear_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#368
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_b#378
    :parameters ()
    :precondition (and (ontable_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_b) (clear_d) (ontable_a) (clear_a) (clear_c) (ontable_d) (ontable_c)
(not (handempty)) (not (ontable_b)) (not (clear_b)))
)


(:action pickup_d#187
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (clear_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#188
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (clear_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#198
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (clear_c) (ontable_d))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#205
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#206
    :parameters ()
    :precondition (and (handempty) (ontable_b) (ontable_a) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#216
    :parameters ()
    :precondition (and (ontable_b) (ontable_a) (ontable_d) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#241
    :parameters ()
    :precondition (and (clear_c) (ontable_a) (clear_b) (clear_d))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#242
    :parameters ()
    :precondition (and (clear_c) (handempty) (ontable_a) (clear_b))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#252
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (ontable_a) (clear_b))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#259
    :parameters ()
    :precondition (and (clear_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#260
    :parameters ()
    :precondition (and (handempty) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#270
    :parameters ()
    :precondition (and (ontable_d) (ontable_a) (clear_b) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#295
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_c) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#296
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_c) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#306
    :parameters ()
    :precondition (and (ontable_b) (clear_c) (clear_a) (ontable_d))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#313
    :parameters ()
    :precondition (and (clear_d) (ontable_b) (clear_a) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#314
    :parameters ()
    :precondition (and (handempty) (ontable_b) (clear_a) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#324
    :parameters ()
    :precondition (and (ontable_b) (clear_a) (ontable_d) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#349
    :parameters ()
    :precondition (and (clear_c) (clear_d) (clear_b) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#350
    :parameters ()
    :precondition (and (clear_c) (handempty) (clear_b) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#360
    :parameters ()
    :precondition (and (clear_c) (ontable_d) (clear_b) (clear_a))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#367
    :parameters ()
    :precondition (and (clear_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#368
    :parameters ()
    :precondition (and (handempty) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


(:action pickup_d#378
    :parameters ()
    :precondition (and (ontable_d) (clear_b) (clear_a) (ontable_c))
    :effect (and (holding_d) (ontable_a) (clear_c) (clear_a) (ontable_b) (clear_b) (ontable_c)
(not (clear_d)) (not (handempty)) (not (ontable_d)))
)


)
