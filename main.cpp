#include <iostream>

/*
This is a test of a more accurate float16 class
Old float 16 class used a 8 bit exponent and a 7 bit mantissa with the sign bit being the MSB of the mantissa.
This was very easy to imlement, but it lacked the required accuracy.

The new float16 has the xact format of the float16 standard, expect that the value is not entirely repressened as a single 16 bit value.
This is the second test with the next to merge the sign bit, exponent and mantissa into a single 16 bit placeholder.
A float value consist of a 5 bit exponent and a 10 bit mantissa.
To represent these values in as few memory spaces as possible, the exponent will be an 8 bit value, only using the 6 LSBs.
The sign bit will be bit 5, and bits 0..4 will be the exponent.
The mantissa will be the 10 MSBs of the 16 bit value, this makes the shifting of the mantissa easier. Any value of less than 2^-9 will be ignored.

The next iteration should not have the need to constantly convert between the final 16 bit value and the the separate exponent and mantissas.
The compresses 16 bit value should only be converted to when it is stored or communicated elsewhere.
*/

using namespace std;

class float16{
	private:
		//Split the 16 bit value into two separate values
		uint8_t valExp; //One for the exponent (contains the sign bit)
		uint16_t valMant; //One for the mantisa (this should show the implicit 2^0 bit
		uint16_t valComp; //This is the compressed 16 bit representation
	public:
		float16();	//Construction of the variable
		void Set_Exp(uint8_t);
		void Set_Mant(uint16_t);
		void Set_Comp(uint16_t);
		void Compress();
		void Decompress();
		uint8_t Get_Exp();
		uint16_t Get_Mant();
		uint16_t Get_Comp();
		void Add_Float16(float16*); //Where the final variable is added to a temporary value of the same construction
};

float16::float16()
{
	//Constructor of type float16
	valExp = 0; //Clear the exponent byte
	valMant = 0; //Clear the matissa byte
}

void float16::Set_Exp(uint8_t tempExp)	{valExp = tempExp;}

void float16::Set_Mant(uint16_t tempMant)	{valMant  = tempMant;}

void float16::Set_Comp(uint16_t tempComp) {valComp = tempComp;}

uint8_t float16::Get_Exp()	{return(valExp);}

uint16_t float16::Get_Mant()	{return(valMant);}

uint16_t float16::Get_Comp() {return(valComp);}

void float16::Compress()
{
	/*This function takes the two variables; uint8_t and uint16_t, and writes it in the float16 format.*/
	std::cout << "Compressing uint8_t exponent and uint16_t mantissa into a custom float16 format (uint16_t)" << endl;//Debug
	
	uint8_t tempExp;
	uint16_t tempMant;
	
	valComp = 0x0000;//Clear the current compressed format since this will be overwritten
	tempExp = valExp;
	tempMant = valMant;
	std::cout << "OldExp: " << static_cast<int>(tempExp) << endl;//Debug
	//Since the mantissa was shifted right to implement the 2^0 bit, this must be reversed.
	//The MSB nibble needs to be removed as well as the MSB of the second nibble
	tempMant = tempMant & 0x07FF;
	valComp = tempExp;//Move the exponent into the compressed variable
	valComp = valComp << 10;//Shift sign and exponent to be 6 MSBs
	std::cout << "NewExp: " << static_cast<int>(valComp) << endl;//Debug
	tempMant = tempMant >> 1; //Shift mantissa to be the 10 LSBs.
	valComp = valComp | tempMant; //Create final float16 value by logical operation
}

void float16::Decompress()
{
	/*This functions takes the variable in the float16 format, and writes it to the uint8_t and uint16_t variables.*/
	std::cout << "Decompressing a float16 (uint16_t) value into a uint8_t exponent and uint16_t mantissa" << endl;//Debug
	
	uint16_t tempExp;//Still contains the sign bit
	uint16_t tempMant;//Shows the implic 2^0 bit 

	std::cout << "Masking exponent:" << endl;
	tempExp = valComp&0xFB00;//Fetch the 6 bits, sign bit and 5 bit exponent
	tempExp = tempExp >> 10;//Shift exponent 10 bits right to fit in 8 bit format
	valExp = static_cast<uint8_t>(tempExp);
	tempMant = valComp & 0x03FF;//Fetch current mantissa
	tempMant = tempMant | 0x0400;//Add implicit 2^0 bit to use in calculations
	tempMant = tempMant << 5;//Shift mantissa 5 bits left to make the mantissa calculations easier
	valMant = tempMant;
}

void float16::Add_Float16(float16* float16Temp)
{
	/*
	This function takes "this" float16 value (uncompressed), and adds to it a temporary float16 value.
	The sum of these two values then overwrites "this" float16 value.
	
	The two float16 variables must already be in their decompressed format.
	The last line calls to compress the sum such that it is already in both formats.
	*/
	
	uint8_t tempExp1, tempExp2;
	uint16_t tempMant1, tempMant2;
	double overflowCheck = 0;
	int loopCount = 1;
	
	tempExp1 = valExp;
	tempExp2 = float16Temp->Get_Exp();
	tempMant1 = valMant;
	tempMant2 = float16Temp->Get_Mant();
	
	//compare exp values
	std::cout << "Compare the exponents of the two variables." << endl;
	while (tempExp1 != tempExp2)
	{
		std::cout << "Loop count: " << loopCount << endl;
		if (tempExp2 <= tempExp1)
		{
			std::cout << "Variable1 exponent is larger" << endl;
			tempExp2++;//Increment exponent value
			tempMant2 = tempMant2 >> 1; //Bit shift mantissa
			std::cout << "Increased old exponent2 to: " << static_cast<int>(tempExp2) << " Shifted old matissa2 to: " << static_cast<int>(tempMant2) << endl;//Debug
		}
		else
		{
			std::cout << "Variable2 exponent is larger" << endl;
			tempExp1++;//Increment exponent value
			tempMant1 = tempMant1 >> 1;//Bit shift mantissa
			std::cout << "Increased old exponent to: " << static_cast<int>(tempExp1) << " Shifted old matissa to: " << static_cast<int>(tempMant1) << endl;//Debug
		}
		loopCount += 1;
	}
	
	cout << "Both exponents are equal" << endl;//Debug
	//add mantissas
	std::cout << "Temp Val1 exponent: " << static_cast<int>(tempExp1) << " Temp Val1 matissa:" << static_cast<int>(tempMant1) << endl;//Debug
	std::cout << "Temp Val2 exponent: " << static_cast<int>(tempExp2) << " Temp Val2 matissa: " << static_cast<int>(tempMant2) << endl;//Debug
	overflowCheck = tempMant1+tempMant2;
	tempMant1 += tempMant2;
	std::cout << "New Temp Val1 matissa: " << static_cast<int>(tempMant1) << endl;//Debug
	
	//check overflow
	if (overflowCheck > 4095)
	{
		std::cout << "Mantissa overflowed" << endl;
		//Add 1 to exponent and shift mantissa right
		tempExp1++;
		tempMant1 = tempMant1 >> 1;
		//Place overflowed bit
		tempMant1 = tempMant1 | 0x8000;
		std::cout << "Temp Val1 exponent: " << static_cast<int>(tempExp1) << " Temp Val1 matissa: " << static_cast<int>(tempMant1) << endl;//Debug
	}
	
	valExp = tempExp1;
	valMant = tempMant1;
	Compress();
}

void convertFloatToFloat16(float fValue, float16* newValue)
{
	int increment = 0;
	bool check = 0;
	bool sign = 0; // Initialises as a positive value
	
	std::cout << fValue << endl;
	//Remove sign
	if (fValue < 0)
	{
		sign = 1;//Set signed bit to negative
		fValue = fValue * -1;
	}
	std::cout << fValue << endl;
	
	//Determine the exponent of the new value
	while (check == 0)
	{
		if (fValue == 0)
		{
			check = 1;
		}
		else if ((fValue >= 1) && (fValue < 2))
		{
			std::cout << "Last: " << increment << ": " << fValue << endl;
			check = 1;
		}
		else if (fValue >= 2)
		{
			std::cout << increment << ": " << fValue << endl;
			fValue = fValue/2;
			increment++;
		}
		else
		{
			std::cout << increment << ": " << fValue << endl;
			fValue = fValue*2;
			increment--;
		}	
	}
	
	//remove the implicit 1
	fValue--;
	uint16_t tempMantissa = 0x00;//Initialise the temporary mantissa as 0x00;
	std::cout << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	//Generate mantissa
	if ((fValue - 0.5) >= 0)
	{
		tempMantissa = tempMantissa | 0x400; //0b0100 0000 0000
		fValue -= 0.5;
		std::cout << "Subtracted 2^-1: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-1
	if ((fValue - 0.25) >= 0)
	{
		tempMantissa = tempMantissa | 0x200; //0b0010 0000 0000
		fValue -= 0.25;
		std::cout << "Subtracted 2^-2: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-2
	if ((fValue - 0.125) >= 0)
	{
		tempMantissa = tempMantissa | 0x100; //0b0001 0000 0000
		fValue -= 0.125;
		std::cout << "Subtracted 2^-3: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-3
	if ((fValue - 0.0625) >= 0)
	{
		tempMantissa = tempMantissa | 0x080; //0b0000 1000 0000
		fValue -= 0.0625;
		std::cout << "Subtracted 2^-4: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-4
	if ((fValue - 0.03125) >= 0)
	{
		tempMantissa = tempMantissa | 0x040; //0b0000 0100 0000
		fValue -= 0.03125;
		std::cout << "Subtracted 2^-5: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-5
	if ((fValue - 0.015625) >= 0)
	{
		tempMantissa = tempMantissa | 0x020; //0b0000 0010 0000
		fValue -= 0.015625;
		std::cout << "Subtracted 2^-6: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-6
	if ((fValue - 0.0078125) >= 0)
	{
		tempMantissa = tempMantissa | 0x010; //0b0000 0001 0000
		fValue -= 0.0078125;
		std::cout << "Subtracted 2^-7: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-7
	if ((fValue - 0.00390625) >= 0)
	{
		tempMantissa = tempMantissa | 0x008; //0b0000 0000 1000
		fValue -= 0.00390625;
		std::cout << "Subtracted 2^-8: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-8
	if ((fValue - 0.001953125) >= 0)
	{
		tempMantissa = tempMantissa | 0x004; //0b0000 0000 0100
		fValue -= 0.001953125;
		std::cout << "Subtracted 2^-9: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-9
	if ((fValue - 0.0009765625) >= 0)
	{
		tempMantissa = tempMantissa | 0x002; //0b0000 0000 0010
		fValue -= 0.0009765625;
		std::cout << "Subtracted 2^-10: Current fValue: " << fValue << " Current mantissa: " << static_cast<int>(tempMantissa) << endl;
	}//check 2^-10

	//Return the 2^0 bit
	tempMantissa = tempMantissa | 0x800;// 0x1000 0000 0000

	newValue->Set_Exp(15+increment);
	uint8_t tempExp;
	tempExp = newValue->Get_Exp();
	
	std::cout << "Old exponent: " << static_cast<int>(tempExp) << endl;//Debug
	if (sign == 1)
	{
		std::cout << "Negative value" << endl;//Debug
		tempExp = tempExp | 0x20; //0b0010 0000
	}
	std::cout << "New exponent: " << static_cast<int>(tempExp) << endl;//Debug
	newValue->Set_Exp(tempExp);
	newValue->Set_Mant(tempMantissa);
	newValue->Compress();
}

int main ()
{
	/*float16 Var1;	//Main test varible
	float16 Var2;	//Temporary float16 value used for calculations

	Var1.Set_Exp(16);
	Var1.Set_Mant(0xC000);
	//Var1 is set to be 0.5
	
	std::cout << "Exponent1: " << static_cast<int>(Var1.Get_Exp()) << " Mantissa1: " << static_cast<int>(Var1.Get_Mant()) << endl;//Debug
	Var1.Compress();
	std::cout << "Compressed as: " << static_cast<int>(Var1.Get_Comp()) << endl;//Debug
	Var1.Set_Exp(0);
	Var1.Set_Mant(0x0000);
	Var1.Decompress();
	std::cout << "Exponent1: " << static_cast<int>(Var1.Get_Exp()) << " Mantissa1: " << static_cast<int>(Var1.Get_Mant()) << endl;//Debug
	
	Var2.Set_Exp(16);
	Var2.Set_Mant(0x8000);
	//Var2 is set to be 1
	
	std::cout << "Exponent2: " << static_cast<int>(Var2.Get_Exp()) << " Mantissa2: " << static_cast<int>(Var2.Get_Mant()) << endl;//Debug
	Var2.Compress();
	std::cout << "Compressed as: " << static_cast<int>(Var2.Get_Comp()) << endl;//Debug
	Var2.Set_Exp(0);
	Var2.Set_Mant(0x0000);
	Var2.Decompress();
	std::cout << "Exponent2: " << static_cast<int>(Var2.Get_Exp()) << " Mantissa2: " << static_cast<int>(Var2.Get_Mant()) << endl;//Debug
	

	Var1.Add_Float16(&Var2);//Add Var2 to Var1 and rewrite it to be the sum
	
	std::cout << "Sum total is:" << endl;
	std::cout << "Exponent1: " << static_cast<int>(Var1.Get_Exp()) << " Mantissa1: " << static_cast<int>(Var1.Get_Mant()) << endl;//Debug
	std::cout << "Compressed as: " << static_cast<int>(Var1.Get_Comp()) << endl;//Debug*/
	
	float A = 0;
	float16 Var3;
	
	while (1)
	{
		Var3.Set_Exp(0);
		Var3.Set_Mant(0);
		Var3.Set_Comp(0);
		
		std::cin >> A;
		if (A != 0)
			convertFloatToFloat16(A,&Var3);
		std::cout << "Exponent: " << static_cast<int>(Var3.Get_Exp()) << " Mantissa: " << static_cast<int>(Var3.Get_Mant()) << endl;//Debug
		std::cout << "Compressed value: " << static_cast<int>(Var3.Get_Comp()) << endl;//Debug
	}
	return(0);
};