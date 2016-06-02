DATE FORMAT (DAT)
-Array of int size 3
-Defaul [0,0,0]

INDI CLASS
-Name: String
-Sex: bool
-Birt: DAT
-Deat: DAT
-FAMC: Pointer to FAM Object
-FAMS: Pointer to FAM Object

FAM CLASS
-MARR: DAT
-HUSB: Pointer to INDI object
-WIFE: Pointer to INDI object
-CHIL: Vector of pointers to INDI Objects
-DIV: DAT
