#include <iostream>
using namespace std;

/*
This is a test of a more accurate float16 class
Old float 16 class used a 8 bit exponent and a 7 bit mantissa with the sign bit being the MSB of the mantissa.
This was very easy to implement, but it lacked the required accuracy.

The new float16 has the exact format of the float16 standard, expect that the value is not entirely represened as a single 16 bit value.

A float value consist of a 5 bit exponent and a 10 bit mantissa.
To represent these values in as few memory spaces as possible, the exponent will be an 8 bit value, only using the 6 LSBs.
The sign bit will be bit 5, and bits 0..4 will be the exponent.
The mantissa will be the 10 MSBs of the 16 bit value, this makes the shifting of the mantissa easier. Any value of less than 2^-9 will be ignored.

Using the Compress fucntion, both sign bit, exponent and mantissa can be repressented as the 16 bit value
This compressed 16 bit value should only be converted to when it is stored or communicated elsewhere.
*/

class float16{
	private:
		//Split the 16 bit value into two separate values
		uint8_t valExp; //One for the exponent (contains the sign bit)	Format of 0bXXSE EEEE
		uint16_t valMant; //One for the mantisa (this should show the implicit 2^0 bit. Format of 0bXXXX XIMM MMMM MMMM
		uint16_t valComp; //This is the compressed 16 bit representation. Format of 0bSEEE EEMM MMMMM MMMM
	public:
		float16();	//Construction of the variable
		void Set_Exp(uint8_t);//Overwrites the current exponent value
		void Set_Mant(uint16_t);//Overwrties the current mantissa value
		void Set_Comp(uint16_t);//Overwrites the current compressed 16 bit representation value
		uint8_t Get_Exp();
		uint16_t Get_Mant();
		uint16_t Get_Comp();
		void Compress();//Takes the exponent and mantissa and create a 16 bit representation
		void Decompress();//Takes the 16 bit representation and create exponent and mantissa values
		void Add_Float16(float16*); //Where 'this' float16 value is added to a temporary float16 value, this overwrites 'this' value
		void Multiply_uint16(uint16_t);//Multiplies 'this' value with an unsigned 16 bit integer value, this overwrites 'this' value
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
	
	uint8_t tempExp;
	uint16_t tempMant;
	
	valComp = 0x0000;//Clear the current compressed format since this will be overwritten
	tempExp = valExp;//Creates a temporary clone of the exponent
	tempMant = valMant;//Creates a temporary clone of the mantissa

	tempMant = tempMant & 0x03FF;//Removes the 6 MSBs (this includes the implicit 2^0 bit)
	valComp = tempExp;//Move the exponent into the compressed variable (this contains the sign bit)
	valComp = valComp << 10;//Shift sign and exponent to be 6 MSBs

	valComp = valComp | tempMant; //Create final float16 value by OR operation
}

void float16::Decompress()
{
	/*This functions takes the variable in the float16 format, and writes it to the uint8_t and uint16_t variables.*/
	
	uint16_t tempExp;//Still contains the sign bit
	uint16_t tempMant;//Shows the implic 2^0 bit
	uint16_t tempComp;//Temporary 16 bit representaion

	tempComp = valComp;//Creates a clone of the 16 bit value
	tempExp = tempComp & 0xFB00;//Fetch the 6 bits, sign bit and 5 bit exponent
	tempExp = tempExp >> 10;//Shift exponent 10 bits right to fit in 8 bit format
	valExp = static_cast<uint8_t>(tempExp);
	
	tempComp = valComp;//Recreates the clone of the old 16 bit value
	tempMant = tempComp & 0x03FF;//Fetch current mantissa
	tempMant = tempMant | 0x0400;//Add implicit 2^0 bit to use in calculations
	valMant = tempMant;
}

void float16::Add_Float16(float16* float16Temp)
{
	/*
	As of 2026-02-11 09h27 UTC+1 this function can't add any negative float16 value
	
	This function takes "this" float16 value (uncompressed), and adds to it a temporary float16 value.
	The sum of these two values then overwrites "this" float16 value.
	
	The two float16 variables must already be in their decompressed format.
	The last line calls to compress the sum such that it is already in both formats.
	
	*/
	
	uint8_t tempExp1, tempExp2;	//Creates copies of both values' exponents
	uint16_t tempMant1, tempMant2;//Creates copies of both values' mantissas
	int loopCount = 1;// A count of how many times the smallest exponents needs to increase before the mantissas can be added
	
	tempExp1 = valExp;//Clones 'this' exponent
	tempExp2 = float16Temp->Get_Exp(); //Clones the temporary exponent
	tempMant1 = valMant;//Clones 'this' mantissa
	tempMant2 = float16Temp->Get_Mant(); //Clones the temporary mantissa
	
	while (tempExp1 != tempExp2)//Loop until both exponents are equal
	{
		std::cout << "Loop count: " << loopCount << endl;
		if (tempExp2 <= tempExp1)
		{
			//This routine doubles the smallest value (in this case the temporary float16 value)
			tempExp2++;//Increment exponent value
			if (tempExp2 > 31)
				break;
			tempMant2 = tempMant2 >> 1; //Bit shift mantissa
		}
		else
		{
			//This routine doubles the smallest value (in this case 'this' float16 value)
			tempExp1++;//Increment exponent value
			if (tempExp1 > 31)
				break;
			tempMant1 = tempMant1 >> 1;//Bit shift mantissa, keeps it 10 bits
		}
		loopCount += 1;
	}
	
	/*//Both mantissas should be shifted so that a overflow can be detected
	tempMant1 = tempMant1 >> 6;
	tempMant2 = tempMant2 >> 6;*/

	tempMant1 += tempMant2;//add mantissas
	if (tempMant1 > 2047)//If a mantissa overflow happens
	{
		tempExp1++;//Add 1 to exponent
		tempMant1 = tempMant1 >> 1;//And shift mantissa right
	}
	
	//tempMant1 = tempMant1 << 6; //Normalise the Mantissa so that the 2^0 bit is the MSB
	valExp = tempExp1;
	valMant = tempMant1;
	std::cout << "newExp: " << static_cast<int>(tempExp1) << " newMant: " << static_cast<int>(tempMant1) << endl;//Debug
}

void float16::Multiply_uint16(uint16_t Multiplicant)
{	
	/*This function multiplies 'this' float16 value with an unsigned 16 bit integer value
	First it has to remove the sign bit  from 'this' value
	Then it has to step through the uint16 value until it equals 0
	Lastly it has to return the sign bit*/

	bool sign = 0;//Initialise the flag as positive
	uint16_t tempMulti = 0;
	float16 oldValue;//This keeps a clone of the old 'this' value before the actual 'this' value become the running sum value
	oldValue.Set_Exp(valExp);
	oldValue.Set_Mant(valMant);
	
	std::cout << "Initial exponent: " << static_cast<int>(valExp) << " Initial mantissa:" <<  static_cast<int>(valMant)<< endl;//Debug
	
	/*Given that the multiplicant is unsigned, it will be good to mask off the sign bit of 'this' value, and add it back in the very end*/
	if (valExp > 31)
		sign = 1;//float16 was a negative value, and the final result will also then be negative
	valExp = valExp & 0x1F;; //Masking off the sign bit
	
	//First to check if the multiplicant is 0
	if (Multiplicant == 0)
	{
		std::cout << "Multiplicant detected to be 0" << endl;//Debug
		//The result should obvisouly be 0
		valExp = 0;
		valMant = 0;
	}
	else if (Multiplicant != 1)
	{	
		while (Multiplicant != 0)
		{
			std::cout << "valExp: " << static_cast<int>(valExp) << " oldExp:" <<  static_cast<int>(oldValue.Get_Exp())<< endl;//Debug
		
			tempMulti = Multiplicant & 0x0001; //Mask the LSB
			
			if (tempMulti == 0x0001) //If the LSB is 1
				Add_Float16(&oldValue);//Here the old float16 value must be added to the current (running sum) float 16 value
			else //Otherwise it has to be 0
				valExp++;//Add 1 to exponent

			Multiplicant = Multiplicant >> 1;
		}
	}
	
	if (sign == 1)
		valExp = valExp | 0x20; //Returns the correct sign bit
}

void convertFloatToFloat16(float fValue, float16* newValue)
{
	int increment = 0;//This is the exponent before normalized into the final exponent (a 15 will be added later)
	bool check = 0; //Used to check if the current float value is between 1 and 2 
	bool sign = 0; //Initialises as a positive value
	
	//Remove sign
	if (fValue < 0)
	{
		sign = 1;//Set signed bit to negative
		fValue = fValue * -1;//Normalize to not work with a negative number
	}
	
	//Determine the exponent of the new value
	while (check == 0)
	{
		if (fValue == 0)
		{
			check = 1;
		}
		else if ((fValue >= 1) && (fValue < 2))
		{
			check = 1;
		}
		else if (fValue >= 2)
		{
			fValue = fValue/2;
			increment++;
		}
		else
		{
			fValue = fValue*2;
			increment--;
		}	
	}
	
	fValue--; //Remove the implicit 1, this bit will be returned after the mantissa is calculated
	uint16_t tempMantissa = 0x00;//Initialise the temporary mantissa as 0x00;

	//Generate mantissa
	if ((fValue - 0.5) >= 0)//check 2^-1
	{
		tempMantissa = tempMantissa | 0x0200; //0b0000 0010 0000 0000
		fValue -= 0.5;
	}
	if ((fValue - 0.25) >= 0)//check 2^-2
	{
		tempMantissa = tempMantissa | 0x0100; //0b0000 0001 0000 0000
		fValue -= 0.25;
	}
	if ((fValue - 0.125) >= 0)//check 2^-3
	{
		tempMantissa = tempMantissa | 0x0080; //0b0000 0000 1000 0000
		fValue -= 0.125;
	}
	if ((fValue - 0.0625) >= 0)//check 2^-4
	{
		tempMantissa = tempMantissa | 0x0040; //0b0000 0000 0100 0000
		fValue -= 0.0625;
	}
	if ((fValue - 0.03125) >= 0)//check 2^-5
	{
		tempMantissa = tempMantissa | 0x0020; //0b0000 0000 0010 0000
		fValue -= 0.03125;
	}
	if ((fValue - 0.015625) >= 0)//check 2^-6
	{
		tempMantissa = tempMantissa | 0x0010; //0b0000 0000 0001 0000
		fValue -= 0.015625;

	}
	if ((fValue - 0.0078125) >= 0)//check 2^-7
	{
		tempMantissa = tempMantissa | 0x0008; //0b0000 0000 0000 1000
		fValue -= 0.0078125;
	}
	if ((fValue - 0.00390625) >= 0)//check 2^-8
	{
		tempMantissa = tempMantissa | 0x0004; //0b0000 0000 0000 0100
		fValue -= 0.00390625;
	}
	if ((fValue - 0.001953125) >= 0)//check 2^-9
	{
		tempMantissa = tempMantissa | 0x0002; //0b0000 0000 000 0010
		fValue -= 0.001953125;
	}
	if ((fValue - 0.0009765625) >= 0)//check 2^-10
	{
		tempMantissa = tempMantissa | 0x0001; //0b0000 0000 0000 0001
		fValue -= 0.0009765625;
	}
	
	tempMantissa = tempMantissa | 0x0400;// Returns the 2^0 bit

	newValue->Set_Exp(15+increment);
	uint8_t tempExp;
	tempExp = newValue->Get_Exp();
	
	if (sign == 1)
		tempExp = tempExp | 0x20; //Flag the sign bit if the value is negative

	newValue->Set_Exp(tempExp);
	newValue->Set_Mant(tempMantissa);
}

int main ()
{
	float A = 0;
	uint16_t B = 0;
	float16 Var3;
	
	while (1)
	{
		Var3.Set_Exp(0);
		Var3.Set_Mant(0);
		Var3.Set_Comp(0);
		
		/*std::cout << "Enter uint16_t multiplicant:" << endl;
		std::cin >> B;*/
		std::cout << "Enter the float value:\t";
		std::cin >> A;
		if (A != 0)
			convertFloatToFloat16(A,&Var3);
		std::cout << "Exponent: " << static_cast<int>(Var3.Get_Exp()) << " Mantissa: " << static_cast<int>(Var3.Get_Mant()) << endl << endl;//Debug
		//std::cout << "Compressed value: " << static_cast<int>(Var3.Get_Comp()) << endl;//Debug
		/*std::cout << "Multiplying..." << endl;
		Var3.Multiply_uint16(B);
		
		std::cout << "New Exponent: " << static_cast<int>(Var3.Get_Exp()) << " New Mantissa: " << static_cast<int>(Var3.Get_Mant()) << endl;//Debug
		//std::cout << "Compressed value: " << static_cast<int>(Var3.Get_Comp()) << endl;//Debug*/
	}
	return(0);
};