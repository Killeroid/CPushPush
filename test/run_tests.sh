LD_LIBRARY_PATH=$LD_LIBRARY_PATH:..
PUSH='./testpush'

# general form of these tests: the thing between brackets is the 
# thing that is tested, the code after it checks if some exit condition is satisfied
# the testpush executable currently just tests the top of the BOOLEAN stack for TRUE after execution,

echo "( ( 5 INTEGER.DUP ) 
	    INTEGER.=) " | $PUSH
echo "( (TRUE FALSE BOOLEAN.DUP ) 
	    BOOLEAN.= ) " | $PUSH
echo "( ( 5 INTEGER.POP ) 
	    INTEGER.STACKDEPTH 0 INTEGER.= ) " | $PUSH
echo "(  (5 6 INTEGER.SWAP ) 
	    5 INTEGER.= 6 INTEGER.=)" | $PUSH
echo "( ( 5 6 INTEGER.= ) 
	    BOOLEAN.NOT)" | $PUSH
echo "( 5 5 INTEGER.= )"| $PUSH

echo "( ( 5 X INTEGER.SET ) 
	    X INTEGER.GET 5 INTEGER.=)" | $PUSH
#echo "( ( 5 X INTEGER.SET) 
#	    X BOOLEAN.GET EXCEPTION.LOOKUPERROR EXCEPTION.=) " | $PUSH

echo "( ( 1 2 3 4 5 1 INTEGER.YANK ) 
	    4 INTEGER.=) " | $PUSH
echo "( ( 1 2 3 4 5 4 INTEGER.YANK ) 
	    1 INTEGER.=) " | $PUSH

echo "( ( 1 2 3 4 5 -21 INTEGER.YANK ) 
	    5 INTEGER.= )" | $PUSH
echo "( ( 1 2 3 4 5 21 INTEGER.YANK )  
	    1 INTEGER.= )" | $PUSH

echo "( ( FALSE TRUE FALSE 1 BOOLEAN.YANK ) )" | $PUSH
echo "( ( FALSE TRUE FALSE 0 BOOLEAN.YANK ) 
	    BOOLEAN.NOT)" | $PUSH

echo "( ( 1 2 3 4 5 -21 INTEGER.YANKDUP ) 
	    5 INTEGER.=)" | $PUSH
echo "( ( 1 2 3 4 5 21 INTEGER.YANKDUP )  
	    1 INTEGER.=)" | $PUSH


echo "( ( FALSE TRUE FALSE 1 BOOLEAN.YANKDUP ) )" | $PUSH
echo "( ( FALSE TRUE FALSE 0 BOOLEAN.YANKDUP ) 
	    BOOLEAN.NOT)" | $PUSH

# factorial
echo " ( 6 CODE.QUOTE (

( CODE.QUOTE ( INTEGER.POP 1 )
            CODE.QUOTE
            ( CODE.DUP INTEGER.DUP 1 INTEGER.- CODE.DO INTEGER.* )
            INTEGER.DUP
            2
            INTEGER.<
            CODE.IF )

) CODE.DO

720 INTEGER.=

)" | $PUSH

# test SHOVE
echo " (
    ( 1 2 3 4 5 1 INTEGER.SHOVE ) 
	4 INTEGER.=
	5 INTEGER.= BOOLEAN.AND
	3 INTEGER.= BOOLEAN.AND
)" | $PUSH

echo "(
    ( 1 2 3 4 5 -100 INTEGER.SHOVE ) 
	5 INTEGER.= 4 INTEGER.= BOOLEAN.AND 3 INTEGER.= BOOLEAN.AND
)"| $PUSH

echo "(
    ( 1 2 3 4 5 100 INTEGER.SHOVE ) 
	INTEGER.STACKDEPTH INTEGER.YANK
	5 INTEGER.=
)"| $PUSH

echo "(
    ( 1 2 3 4 5 4 INTEGER.SHOVE ) 
	4 INTEGER.YANK
	5 INTEGER.=
)"| $PUSH

# test STACKDEPTH
echo "(
    ( TRUE TRUE TRUE TRUE TRUE BOOLEAN.STACKDEPTH ) 
    5 INTEGER.=
)"| $PUSH

echo "(
    ( 1 2 3 4 5 INTEGER.STACKDEPTH ) 
    5 INTEGER.=
)"| $PUSH

# test QUOTE

echo "(
    ( CODE.QUOTE ( FOO BAR ) ) 
    CODE.ATOM BOOLEAN.NOT
)" | $PUSH

echo "(
    ( CODE.QUOTE FOO  ) 
    CODE.ATOM
)" | $PUSH

echo "(
    ( CODE.QUOTE ()  ) 
    CODE.NULL
)" | $PUSH

echo "(
    ( CODE.QUOTE 22  ) 
    CODE.ATOM
)" | $PUSH

echo "(
    ( CODE.QUOTE (FOO BAR)  ) 
    CODE.NULL BOOLEAN.NOT
)" | $PUSH

echo "(
    ( CODE.QUOTE ( FOO BAR BAZ ) CODE.CAR ) 
    CODE.QUOTE FOO CODE.=
)" | $PUSH

echo "(
    ( CODE.QUOTE ( FOO BAR BAZ ) CODE.CDR ) 
    CODE.QUOTE (BAR BAZ) CODE.=
)" | $PUSH

echo "(
    ( CODE.QUOTE BONK CODE.QUOTE ( FOO BAR BAZ ) CODE.CONS ) 
    CODE.QUOTE (BONK FOO BAR BAZ) CODE.=
)" | $PUSH

# checks if 'CODE.DO' is one of the instructions
echo "(
    ( CODE.INSTRUCTIONS ) 
    CODE.QUOTE CODE.DO CODE.SWAP CODE.MEMBER
)" | $PUSH

echo "(
    ( CODE.QUOTE BONK CODE.QUOTE FOO CODE.CONS ) 
    CODE.QUOTE (BONK FOO) CODE.=
)" | $PUSH

echo "(
    ( CODE.QUOTE ( CODE.QUOTE ( FOO BAR BAZ ) CODE.CAR ) CODE.DO* ) 
    TRUE 
)" | $PUSH

echo "(
    CODE.QUOTE (1 (3 4 5) 6 (8))
    2 CODE.EXTRACT CODE.QUOTE (3 4 5) CODE.=
)" | $PUSH

echo "(
    CODE.QUOTE FOO
    CODE.QUOTE (1 (3 4 5) 6)
    2 CODE.INSERT CODE.QUOTE (1 FOO 6) CODE.=
)" | $PUSH

# equality of instructions
echo "(
    CODE.QUOTE CODE.QUOTE
    CODE.QUOTE CODE.DO
    CODE.= BOOLEAN.NOT
    )" | $PUSH

echo "(
    CODE.QUOTE CODE.QUOTE
    CODE.QUOTE CODE.QUOTE
    CODE.= 
    )" | $PUSH

# nth
echo " (
( CODE.QUOTE ( A B C D ) 2 CODE.NTH ) 
    
    CODE.QUOTE C CODE.= 
    
)" |$PUSH  


# Ackermann and other 'drop in' push test programs
for file in *.push
do
    $PUSH < $file
    if [ $? -eq 1 ] 
    then
	echo "Error in $file"
	exit 1
    fi
done

# DEFINE
echo "(
	hi EXEC.DEFINE 1 
	hi
	1 INTEGER.=
	)" | $PUSH

exit 0
( CODE.QUOTE ( CODE.QUOTE ( FOO BAR BAZ ) CODE.CAR ) CODE.DO* ) 
( CODE.NOOP ) 
( CODE.QUOTE FOO CODE.QUOTE BAR CODE.LIST ) 
( CODE.QUOTE ( FOO ) CODE.QUOTE ( BAR ) CODE.APPEND ) 
( CODE.QUOTE FOO CODE.QUOTE BAR CODE.APPEND ) 
( CODE.QUOTE ( A B C D ) 2000 CODE.NTH ) 
( CODE.QUOTE ( A B C D ) 2 CODE.NTHCDR ) 
( CODE.QUOTE ( A B C D ) 2000 CODE.NTHCDR ) 
( CODE.QUOTE ( A B ) CODE.QUOTE ( X ( A B ) Y ) CODE.MEMBER ) 
( CODE.QUOTE ( A B ) CODE.QUOTE ( X ( A X ) Y ) CODE.MEMBER ) 
( CODE.QUOTE ( A B ) CODE.QUOTE ( X ( A B ) Y ) CODE.POSITION ) 
( CODE.QUOTE ( A B ) CODE.QUOTE ( X ( A X ) Y ) CODE.POSITION ) 
( CODE.QUOTE ( X ( B ( A ) ) ) CODE.QUOTE ( A ) CODE.CONTAINS ) 
( CODE.QUOTE ( X ( B ( Z ) ) ) CODE.QUOTE ( A ) CODE.CONTAINS ) 
( CODE.QUOTE ( X Y Z ) CODE.QUOTE A 0 CODE.INSERT ) 
( CODE.QUOTE ( X Y Z ) CODE.QUOTE A 1 CODE.INSERT ) 
( CODE.QUOTE ( X Y Z ) CODE.QUOTE A 3 CODE.INSERT ) 
( CODE.QUOTE ( X ( Y ) Z ) 0 CODE.EXTRACT ) 
( CODE.QUOTE ( X ( Y ) Z ) 1 CODE.EXTRACT ) 
( CODE.QUOTE ( X ( Y ) Z ) 2 CODE.EXTRACT ) 
( CODE.QUOTE ( X ( Y ) Z ) 3 CODE.EXTRACT ) 
( CODE.QUOTE ( X ( Y ) Z ) 5 CODE.EXTRACT ) 
( CODE.LENGTH ) 
( CODE.QUOTE ( A B C ) CODE.SIZE ) 
( CODE.QUOTE ( CODE.QUOTE ( FOO BAR BAZ ) CODE.CAR ) CODE.DO ) 
( CODE.QUOTE ( TRUE FALSE ) CODE.DO ) 
( 2 3 INTEGER.+ ) 
( 5 2 INTEGER.- ) 
( 2 3 INTEGER.* ) 
( 10 3 INTEGER./ ) 
( 10 0 INTEGER./ ) 
( 10 3 INTEGER.% ) 
( 10 0 INTEGER.% ) 
( 10 3 INTEGER.MIN ) 
( 10 3 INTEGER.MAX ) 
( 10 5 INTEGER.< ) 
( 5 10 INTEGER.< ) 
( 10 5 INTEGER.> ) 
( 5 10 INTEGER.> ) 
( FALSE TRUE BOOLEAN.OR FALSE BOOLEAN.NOT BOOLEAN.AND ) 
( FALSE TRUE BOOLEAN.AND FALSE BOOLEAN.NOT BOOLEAN.AND ) 
( FALSE TRUE BOOLEAN.OR TRUE BOOLEAN.NOT BOOLEAN.AND ) 
( 1.2 FLOAT.SIN 2.3 FLOAT.COS 4.5 FLOAT.TAN FLOAT.+ FLOAT.* ) 

