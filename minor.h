/* minor.h (acdc) - copyleft Mike Arnautov 1990-2011.
 *
 * 06 Oct 10   MLA           Added RESAY.
 * 15 Mar 08   MLA           Version 12 changes.
 * 10 Nov 07   MLA           Added DUMP.
 * 01 Jan 05   MLA           Added UNDO/REDO.
 * 20 Aug 04   MLA           Added IFCGI and IFDOALL
 * 27 Jul 02   MLA           Added CHECKPOINT.
 * 12 Jul 02   MLA           Added IFNE, IFLE, IFGE, DESCRIBE.
 * 01 Jan 02   MLA           Added LOCAL.
 * 17 May 01   MLA           Added FAKECOM.
 * 16 Mar 01   MLA           Added IFANY.
 * 05 Mar 01   MLA           Added FAKEARG.
 * 28 Feb 01   MLA           Added TIE.
 * 08 Aug 00   MLA           Added OTHERWISE.
 * 15 Sep 90   MLA           Initial coding.
 *
 * The below definitions are only used internally by acdc and hence
 * can be re-arranged and re-numbered at will.
 */

# define IFEQ         1
# define IFNE         2
# define IFLT         3
# define IFLE         4
# define IFGT         5
# define IFGE         6
# define IFAT         7
# define IFHAVE       8
# define IFNEAR       9
# define IFHERE      10
# define IFLOC       11
# define IFIS        12
# define IFINRANGE   13
# define IFKEY       14
# define IFANY       15
# define IFFLAG      16
# define IFHTML      17
# define IFCGI       18
# define IFDOALL     19
# define IFTYPED     20
# define CHANCE      21
# define QUERY       22

# define NOT         25
# define AND         26
# define OR          27
# define XOR         28

# define ELSE        31
# define FIN         32
# define EOT         33
# define OTHERWISE   34

# define KEYWORD     40
# define HAVE        41
# define NEAR        42
# define HERE        43
# define ATLOC       44
# define ANYOF       45

# define ITOBJ       51
# define ITLOC       52
# define ITERATE     53
# define NEXT        54
# define BREAK       55
# define EOI         56

# define EXEC        57
# define FAKEARG     58
# define FAKECOM     59

# define CALL        61
# define PROCEED     62
# define QUIT        63
# define STOP        64 

# define GOTO        71
# define MOVE        72
# define SMOVE       73

# define SAY         81
# define DESCRIBE    82
# define VALUE       83
# define QUIP        84
# define RESPOND     85
# define DICT        86
# define APPEND      87
# define RESAY       88

# define FLAG        91
# define UNFLAG      92
# define SVAR        93
# define INPUT       94
# define DEFAULT     95
# define SET         96
# define DOALL       97
# define FLUSH       98

# define ADD        101
# define SUBTRACT   102
# define MULTIPLY   103
# define DIVIDE     104
# define NEGATE     105
# define RANDOM     106
# define CHOOSE     107
# define RANDOMISE  108
# define LDA        109
# define EVAL       110
# define DEPOSIT    111
# define INTERSECT  112
# define TIE        113

# define GET        121
# define DROP       122
# define APPORT     123
# define LOCATE     124

# define LOCAL      131

# define CHECKPOINT 141
# define DUMP       142

# define SAVE       151
# define RESTORE    152
# define DELETE     153
# define SAVEDLIST  154

# define VERBATIM   161

# define UNDO       171
# define REDO       172
