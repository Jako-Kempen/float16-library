ORG 0H;
MOV R4,#0x3F;Saves Multi as 7
MOV A, #0x47;Saves M_Exp as 71
RLC A;Bitshift (w CY) left current M_Exp
;MOV R5 @0xD0; Copies sign bit into CY bit
RR A;Bitshift (wo CY) right current M_Exp
CLR CY;Clears CY
MOV R0,A;Copies unsigned M_Exp into R0
CLR A;Clear A to reset R_Exp and R_Mant
MOV R2,A;Clears R_Exp
MOV R3,A;Clears R_Mant
MOV R1,#0xFF;Saves M_Mant as 2^0
MOV A,R4;Copies Multi into A
JZ Exit;Multi = 0
MOV A,R0;
MOV R2,A; R_Mant = M_Mant
MOV A,R4
Null_Check: JZ Exit;Jumps to Exit (Multi = 0)
JB ACC.0, Add_Mant;Running sum = Memory
RR A;Bitshift right Multi (Div 2)
INC R0;Increment M_Exp
MOV R4,A;Copies Multi into R4
SJMP Null_Check;Jumps back to Multi = 0?
Add_Mant: DEC A;Multi =-1
MOV R4,A;Copies Multi into R4
MOV A,R2
MOV B,R0
Check_Exp: CJNE A,B,Normalize
MOV A,R3;Copies R_Mant into A
ADD A,R1;R_Mant += M_Mant
MOV R3,A
JB CY, Running_Shift;Overflow detected
Multi_Reset:MOV R3,A;Copies Copies A into R_Mant
MOV A,R4;Copies Multi into A
SJMP Null_Check;Jumps back to Multi = 0?
Running_Shift: RRC A;Bitshift (w CY) right (normalise R_Mant)
CLR CY;Clears CY
INC R2;Increase R_Exp
SJMP Multi_Reset;Jumps to Multi_Reset
Normalize: INC R2
MOV A,R3
RR A
MOV R3,A
MOV A,R2
SJMP Check_Exp
Exit: MOV A,R2;Copies R_Exp into A
RL A;Prepare R_Exp for signbit placement
RRC A;Place the signbit into R_Exp
MOV R2, A;Places A into R_Exp
END
