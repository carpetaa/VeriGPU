li x2, 7
li x3, 11

slt x1, x2, x3
outr x1  ; should be 1

slt x1, x3, x2
outr x1  ; should be 0

slt x1, x2, x2
outr x1  ; should be 0

sltu x1, x0, x2
outr x1  ; should be 1

li x2, 0
sltu x1, x0, x2
outr x1  ; should be 0

li x2, -1
sltu x1, x0, x2
outr x1  ; should be 1

li x2, 0b1011
li x3, 0b0110
and x1, x2, x3
outr x1  ; 0b0010

# try add, sll, srl

li x1 15
li x2 8
add x3, x1, x2
outr x3

li x2 1
sll x3, x1, x2
outr x3

li x2 2
sll x3, x1, x2
outr x3

li x2 3
sll x3, x1, x2
outr x3

li x2 1
srl x3, x1, x2
outr x3

li x2 2
srl x3, x1, x2
outr x3

li x2 3
srl x3, x1, x2
outr x3

or x3, x1, x2
outr x3

xor x3, x1, x2
outr x3

sub x3, x2, x1
outr x3

li x1 -150
li x2 3
sra x3, x1, x2
outr x3

halt
