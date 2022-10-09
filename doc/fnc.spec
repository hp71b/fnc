exemple :

function areuh ( a, b, c)
{
    variable d, e, f,
             g, h ;

    d = a*b+c ;
    return d % (a -b) ;
}



<fonction> ::=
	<entete> <corps>

<entete> ::=
	function <id> ( <liste-par> )

<liste-par> ::=
	nil
	<liste-var>

<liste-var> ::=
	id
	id , <liste-var>

<corps> ::=
	{ <liste-instructions> <retour> }
	{ <declarations> <liste-instructions> <retour> }

<declarations> ::=
	variable <liste-var> ;

<liste-instructions> ::=
	<instruction>
	<instruction> <liste-instructions>

<instruction> ::=
	<id> = <expression> ;

<retour> ::=
	return <expression> ;

<expression> ::=
	<terme> + <expression>
	<terme> - <expression>
	<terme>

<terme> ::=
	<facteur> * <expression>
	<facteur> / <expression>
	<facteur> % <expression>
	<facteur>

<facteur> ::=
	- <facteur>
	<primaire>

<primaire> ::=
	<id>
	<const>
	( <expression> )
