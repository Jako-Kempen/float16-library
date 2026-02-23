#include <iostream>
#include <fstream>
#include <string>
using namespace std;

/*
This is a test of a more accurate float16 class
Old float 16 class used a 8 bit exponent and a 7 bit mantissa with the sign bit being the MSB of the mantissa.
This was very easy to implement, but it lacked the required accuracy.

The new float16 has the exact format of the float16 standard, except that the value is not entirely represened as a single 16 bit value.

*Standardized float16*
A float value consist of a 5 bit exponent and a 10 bit mantissa.
To represent these values in as few memory spaces as possible, the exponent will be an 8 bit value, only using the 6 LSBs.
The sign bit will be bit 5, and bits 0..4 will be the exponent.
The mantissa will be the 10 MSBs of the 16 bit value, this makes the shifting of the mantissa easier. Any value of less than 2^-9 will be ignored.

Using the Compress function, both sign bit, exponent and mantissa can be repressented as the 16 bit value
This compressed 16 bit value should only be converted to when it is stored or communicated elsewhere.

2026-02-23 12:00:00 (UTC+1)
Code will be changed so that it now uses 32 bits to represent a value,
The exponent will be 7 bits, with a separate sign bit, for a total of 8 bits used of a 16 bit memory location
The mantissa will be 14 bits, with separate bits for the 2^0 bit, and an overflow bit used for the summation of two float16 values
float16 is not an accurate name anymore, but the name will stay the same as both parts still occupy 16 bit separately.
*/

class float16{
	private:
		//Split the 16 bit value into two separate values
		uint16_t valExp; //One for the exponent (contains the sign bit)	Format of 0b 0000 0000 SEEE EEEE
		uint16_t valMant; //One for the mantisa (this should show the implicit 2^0 bit. Format of 0bOIMM MMMM MMMM MMMM
		uint32_t valComp; //This is the compressed 32 bit representation. Format TBD
	public:
		float16();	//Construction of the variable
		void Set_Exp(uint16_t);//Overwrites the current exponent value
		void Set_Mant(uint16_t);//Overwrites the current mantissa value
		void Set_Comp(uint32_t);//Overwrites the current compressed 32 bit representation value
		uint16_t Get_Exp();
		uint16_t Get_Mant();
		uint32_t Get_Comp();
		void Compress();//Takes the exponent and mantissa and create a 32 bit representation
		void Decompress();//Takes the 32 bit representation and create exponent and mantissa values
		void Add_Float16(float16); //Where 'this' float16 value is added to a temporary float16 value, this overwrites 'this' value
		void Multiply_uint16(uint16_t);//Multiplies 'this' value with an unsigned 16 bit integer value, this overwrites 'this' value
};

float16::float16()
{
	//Constructor of type float16
	valExp = 0; //Clear the exponent byte
	valMant = 0; //Clear the matissa byte
}

void float16::Set_Exp(uint16_t tempExp)	{valExp = tempExp;}

void float16::Set_Mant(uint16_t tempMant)	{valMant  = tempMant;}

void float16::Set_Comp(uint32_t tempComp) {valComp = tempComp;}

uint16_t float16::Get_Exp()	{return(valExp);}

uint16_t float16::Get_Mant()	{return(valMant);}

uint32_t float16::Get_Comp() {return(valComp);}

void float16::Compress()
{
	/*This function takes the two variables; uint8_t and uint16_t, and writes it in the float16 format.*/
	
	uint16_t tempExp;
	uint16_t tempMant;
	
	valComp = 0x00000000;//Clear the current compressed format since this will be overwritten
	tempExp = valExp;//Creates a temporary clone of the exponent
	tempMant = valMant;//Creates a temporary clone of the mantissa
	/*Format TBD
	tempMant = tempMant & 0x03FF;//Removes the 6 MSBs (this includes the implicit 2^0 bit)
	valComp = tempExp;//Move the exponent into the compressed variable (this contains the sign bit)
	valComp = valComp << 10;//Shift sign and exponent to be 6 MSBs
	*/
	valComp = valComp | tempMant; //Create final float16 value by OR operation
}

void float16::Decompress()
{
	/*This functions takes the variable in the float16 format, and writes it to the uint8_t and uint16_t variables.*/
	
	uint16_t tempExp;//Still contains the sign bit
	uint16_t tempMant;//Shows the implic 2^0 bit
	uint32_t tempComp;//Temporary 16 bit representaion
	/*Format TBD
	tempComp = valComp;//Creates a clone of the 16 bit value
	tempExp = tempComp & 0xFB00;//Fetch the 6 bits, sign bit and 5 bit exponent
	tempExp = tempExp >> 10;//Shift exponent 10 bits right to fit in 8 bit format
	valExp = static_cast<uint8_t>(tempExp);
	
	tempComp = valComp;//Recreates the clone of the old 16 bit value
	tempMant = tempComp & 0x03FF;//Fetch current mantissa
	tempMant = tempMant | 0x0400;//Add implicit 2^0 bit to use in calculations
	valMant = tempMant;
	*/
}

void float16::Add_Float16(float16 float16Temp)
{
	/*
	As of 2026-02-11 09h27 UTC+1 this function can't add any negative float16 value
	
	This function takes "this" float16 value (uncompressed), and adds to it a temporary float16 value.
	The sum of these two values then overwrites "this" float16 value.
	
	The two float16 variables must already be in their decompressed format.
	*/
	
	uint16_t tempExp1, tempExp2;	//Creates copies of both values' exponents
	uint16_t tempMant1, tempMant2;//Creates copies of both values' mantissas
	int loopCount = 1;// A count of how many times the smallest exponents needs to increase before the mantissas can be added
	bool isZero = 0;//Dettected that one of the values is a zero
	
	tempExp1 = valExp;//Clones 'this' exponent
	tempExp2 = float16Temp.Get_Exp(); //Clones the temporary exponent
	tempMant1 = valMant;//Clones 'this' mantissa
	tempMant2 = float16Temp.Get_Mant(); //Clones the temporary mantissa
	//Compress();
	//float16Temp->Compress();
	
	if ((tempExp1 == 0) && (tempMant1 == 0))
	{
		valExp = tempExp2;
		valMant = tempMant2;
		//std::cout << "Value 1 was 0" << endl;//Debug
	}
	else if ((tempExp2 == 0) && (tempMant2 == 0))
	{
		valExp = tempExp1;
		valMant = tempMant1;
		//std::cout << "Value 2 was 0" << endl;//Debug
	}
	else
	{
	while (tempExp1 != tempExp2)//Loop until both exponents are equal
	{
		//std::cout << "Loop count: " << loopCount << endl;
		if (tempExp2 <= tempExp1)
		{
			//This routine doubles the smallest value (in this case the temporary float16 value)
			tempExp2++;//Increment exponent value
			if (tempExp2 > 63)
				break;
			tempMant2 = tempMant2 >> 1; //Bit shift mantissa
		}
		else
		{
			//This routine doubles the smallest value (in this case 'this' float16 value)
			tempExp1++;//Increment exponent value
			if (tempExp1 > 63)
				break;
			tempMant1 = tempMant1 >> 1;//Bit shift mantissa, keeps it 10 bits
		}
		loopCount += 1;
	}

	tempMant1 += tempMant2;//add mantissas
	if (tempMant1 > 16383)//If a mantissa overflow happens
	{
		tempExp1++;//Add 1 to exponent
		tempMant1 = tempMant1 >> 1;//And shift mantissa right
	}
	
	valExp = tempExp1;
	valMant = tempMant1;
	}
	//std::cout << "newExp: " << static_cast<int>(tempExp1) << " newMant: " << static_cast<int>(tempMant1) << endl;//Debug
}

void float16::Multiply_uint16(uint16_t Multiplicant)
{	
	/*This function multiplies 'this' float16 value with an unsigned 16 bit integer value
	First it has to remove the sign bit  from 'this' value
	Then it has to step through the uint16 value until it equals 0
	Lastly it has to return the sign bit*/

	bool sign = 0;//Initialise the flag as positive
	uint16_t tempMulti = 0;
	float16 temp;//This is the value that will be adjusted as it steps through the multiplicant
	
	/*Given that the multiplicant is unsigned, it will be good to mask off the sign bit of 'this' value, and add it back in the very end*/
	if (valExp > 63)
		sign = 1;//float16 was a negative value, and the final result will also then be negative
	valExp = valExp & 0x1F;; //Masking off the sign bit
	
	temp.Set_Exp(valExp);//It will be set as a clone of 'this' variable
	temp.Set_Mant(valMant);//This is done before the 'this' value is set to 0, as it becomes the running sum
	valExp = 0;
	valMant = 0;
	
	//Loop through the multiplicant
	while (Multiplicant != 0)
	{
		tempMulti = Multiplicant & 0x0001;//Mask the LSB
		if (tempMulti == 0x0001)//If the multiplicant is odd
		{
			Add_Float16(temp);//Add the temporary float16 value to the running sum
			Multiplicant--;//decrement the multiplicant to make it an even number
		}
		else
		{
			Multiplicant = Multiplicant >> 1;//Divide the multiplicant in half
			temp.Set_Exp(temp.Get_Exp()+1);//Increment the temporary exponent (doubles it)
		}
	}//Do so until the entire multiplicant has be stepped through
	
	if (sign == 1)
		valExp = valExp | 0x80; //Returns the correct sign bit
}

void openFile()
{
	/*This function opens the file with the name of the variable.
	If this file does not exist, it should not be created, and the user should be informed that there is no file with the given name
	*/
	
	
}

void closeFile()
{
	//THis function closes the file with the name of the variable (or file type)
}	

void readLine()
{
	//This function reads a line from the given open file. It returns 1 if the end of file is reached.
}

void parseLine(string data, float16* saveMemory, uint16_t* samplePointer)
{
	/*
	'data' contains the entire line as read from the file
	'saveMemory'is the block containing the 20 float16 values as read from the file
	'sample' is the first entry of the line, this represents the ADC 12 bit sampled value
	*/
	
	//This function is depedent on what the info is it should parse.
	/* In this case, the first value is the 12 bit sample, and the next values should be deemed as a pair.
	The first value of the pair is the exponent value, and the second is the mantissa.
	It should then ut these values into their separete variable. The samples should be placed in a uint16_t variable,
	and the subsequent values are stored in a 2 dimentional array.
	Once all data in the line is parsed, it should jump to the next function that multiplies the sample with each pair value
	and then after each multiplication, it should add the calculated value to a running sum.
	*/
	
	uint8_t commaPosition = 0;//This is the location of the  first ',' character in string data
	string text;//This is the placeholder for the text to be processed
	char value[6];//This represents the value as a string
	std::size_t Len;//The length of 'value' string
	uint8_t loop = 0; //Always start at memory position 0
	uint16_t sample;

	//Sample
	commaPosition = data.find(',');//Find the position of the ',' character
	//cout << "Comma was found at index: " << static_cast<int>(commaPosition) << endl;//Debug
	
	//Process the text up to the position of the ',' character
	Len = data.copy(value,commaPosition);
	value[Len] = '\0';
	int temp(stoi(value));
	sample = static_cast<uint16_t>(temp);//sample now contains the uint16_t value as read from the file
	//samplePointer is the address, and to change the value at the address:
	*samplePointer = sample;//This should change the value of 'sample'
	
	//Remove the portion already processed
	data = data.substr(commaPosition+1);//Removes all characters upto and including the ',' character
	
	
	while (loop < 20)
	{
		//Find the position of the next ',' character
		commaPosition = data.find(',');
		//Process the text up to the position of the ',' character
		Len = data.copy(value,commaPosition);
		value[Len] = '\0';
		//convert string to uint16_t
		int temp1(stoi(value));//Temporary placeholder (Converts string to temporary int)
		saveMemory[loop].Set_Exp(static_cast<uint8_t>(temp1));
		//cout << "Sample: " <<sample << endl;//Debug: display sample value (uint16_t)
	
		//Remove the portion already processed
		data = data.substr(commaPosition+1);
		//cout << data << endl << endl;//Debug: Displays the remainder if the string
	
	
		//Find the position of the next ',' character
		commaPosition = data.find(',');
		//Process the text up to the position of the ',' character
		Len = data.copy(value,commaPosition);
		value[Len] = '\0';
		//convert string to uint16_t
		int temp2(stoi(value));//Temporary placeholder (Converts string to temporary int)
		saveMemory[loop].Set_Mant(static_cast<uint16_t>(temp2));
		//cout << "Sample: " <<sample << endl;//Debug: display sample value (uint16_t)
	
		//Remove the portion already processed
		data = data.substr(commaPosition+1);
		//cout << data << endl << endl;//Debug
		loop++;
	}//End while not end of line
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
	uint16_t tempMantissa = 0x0000;//Initialise the temporary mantissa as 0x00;

	//Generate mantissa
	if ((fValue - 0.5) >= 0)//check 2^-1
	{
		tempMantissa = tempMantissa | 0x2000; //0b0010 0000 0000 0000
		fValue -= 0.5;
	}
	if ((fValue - 0.25) >= 0)//check 2^-2
	{
		tempMantissa = tempMantissa | 0x1000; //0b0001 0000 0000 0000
		fValue -= 0.25;
	}
	if ((fValue - 0.125) >= 0)//check 2^-3
	{
		tempMantissa = tempMantissa | 0x0800; //0b0000 1000 0000 0000
		fValue -= 0.125;
	}
	if ((fValue - 0.0625) >= 0)//check 2^-4
	{
		tempMantissa = tempMantissa | 0x0400; //0b0000 0100 0000 0000
		fValue -= 0.0625;
	}
	if ((fValue - 0.03125) >= 0)//check 2^-5
	{
		tempMantissa = tempMantissa | 0x0200; //0b0000 0010 0000 0000
		fValue -= 0.03125;
	}
	if ((fValue - 0.015625) >= 0)//check 2^-6
	{
		tempMantissa = tempMantissa | 0x0100; //0b0000 0001 0000 0000
		fValue -= 0.015625;

	}
	if ((fValue - 0.0078125) >= 0)//check 2^-7
	{
		tempMantissa = tempMantissa | 0x0080; //0b0000 0000 1000 0000
		fValue -= 0.0078125;
	}
	if ((fValue - 0.00390625) >= 0)//check 2^-8
	{
		tempMantissa = tempMantissa | 0x0040; //0b0000 0000 0100 0000
		fValue -= 0.00390625;
	}
	if ((fValue - 0.001953125) >= 0)//check 2^-9
	{
		tempMantissa = tempMantissa | 0x0020; //0b0000 0000 0010 0000
		fValue -= 0.001953125;
	}
	if ((fValue - 0.0009765625) >= 0)//check 2^-10
	{
		tempMantissa = tempMantissa | 0x0010; //0b0000 0000 0001 0000
		fValue -= 0.0009765625;
	}
		if ((fValue - 0.00048828125) >= 0)//check 2^-11
	{
		tempMantissa = tempMantissa | 0x0080; //0b0000 0000 0000 1000
		fValue -= 0.00048828125;
	}
	if ((fValue - 0,000244140625) >= 0)//check 2^-12
	{
		tempMantissa = tempMantissa | 0x0040; //0b0000 0000 0000 0100
		fValue -= 0.000244140625;
	}
	if ((fValue - 0.0001220703125 >= 0)//check 2^-13
	{
		tempMantissa = tempMantissa | 0x0020; //0b0000 0000 0000 0010
		fValue -= 0.0001220703125;
	}
	if ((fValue - 0.00006103515625) >= 0)//check 2^-14
	{
		tempMantissa = tempMantissa | 0x0010; //0b0000 0000 0000 0001
		fValue -= 0.00006103515625;
	}
	
	tempMantissa = tempMantissa | 0x4000;// Returns the 2^0 bit

	newValue->Set_Exp(31+increment);
	uint16_t tempExp;
	tempExp = newValue->Get_Exp();
	
	if (sign == 1)
		tempExp = tempExp | 0x80; //Flag the sign bit if the value is negative

	newValue->Set_Exp(tempExp);
	newValue->Set_Mant(tempMantissa);
}

void Multiply(float16* runningSum, uint16_t Multiplier, float16* memory)
{
	/*
	This function takes the Multiplier and times it with each value in memory.
	The result of each multiplication is added to the current value of the running sum
	*/
	
	float16 tempMemory;
	tempMemory.Set_Exp(0);
	tempMemory.Set_Mant(0);
	
	//This steps through each memory entry in the 20 sized array (this is a row in the memory)
	for (int iteration = 0; iteration < 20; iteration++)
	{
		tempMemory.Set_Exp(memory[iteration].Get_Exp());
		tempMemory.Set_Mant(memory[iteration].Get_Mant());
		
		//cout << "Memory Exp:" << static_cast<int>(tempMemory.Get_Exp()) << " Memory Mant: " << static_cast<int>(tempMemory.Get_Mant()) << endl;
		if ((tempMemory.Get_Exp() == 0) && (tempMemory.Get_Mant() == 0))
			cout << "Memory is 0." << endl;
		else
			tempMemory.Multiply_uint16(Multiplier);
		cout << "For index: " << iteration;
		cout << "Multiplied Exp:" << static_cast<int>(tempMemory.Get_Exp()) << " Multiplied Mant: " << static_cast<int>(tempMemory.Get_Mant()) << endl;
		runningSum[iteration].Add_Float16(tempMemory);
		cout << "Running Sum Exp:" << static_cast<int>(runningSum[iteration].Get_Exp()) << " Runnings Sum Mant: " << static_cast<int>(runningSum[iteration].Get_Mant()) << endl;
	}
}

void Multiply40SamplesWithMemory()
{
	float16 memory;
	float16 sum;
	uint16_t multiplier;
	
	sum.Set_Exp(0);
	sum.Set_Mant(0);
	
	int tempExp;
	int tempMant;
	
	for (int count =  0; count < 40; count++)
	{
		cout << "Line: " << count << endl;
		cout << "Enter multiplier: ";
		cin >> multiplier;
	
		cout << "Enter Exponent: ";
		cin >> tempExp;
		
		cout << "Enter mantissa: ";
		cin >> tempMant;
		
		memory.Set_Exp(static_cast<uint8_t>(tempExp));
		memory.Set_Mant(static_cast<uint16_t>(tempMant));
		
		memory.Multiply_uint16(multiplier);
		cout << "Multiplication result -> Exp: " << static_cast<int>(memory.Get_Exp()) << " Mant: " << static_cast<int>(memory.Get_Mant()) << endl;
		
		sum.Add_Float16(memory);
		cout << "Running sum result -> Exp: " << static_cast<int>(sum.Get_Exp()) << " Mant: " << static_cast<int>(sum.Get_Mant()) << endl;
		cout << endl << endl;
		
	}
}

int main()
{
	//Multiply40SamplesWithMemory();
	float A;
	float16* B;
	string line;
	uint8_t commaPosition = 0;
	char value[15];
	std::size_t valueLength;
	
	uint8_t tExp;
	uint16_t tMant;
	
	std::cin >> A;
	std::cout << sizeof(A) << endl;
	
	//Open Read file
	ifstream readFile ("Read.csv");
	
	//Open write file
	ofstream writeFile;
	writeFile.open("Write.csv");
	
	/*while (1)
	{*/
	if (readFile.is_open())
	{
		while (getline(readFile,line))
		{
				//for loop parse data
				for (int step = 0; step < 20; step++)
				{
					if (line.find(',') != line.npos)
						commaPosition = line.find(',');
					else
						
					valueLength = line.copy(value,commaPosition);
					value[valueLength] = '\0';
					int temp(stof(value));
					
					B->Set_Exp(0);
					B->Set_Mant(0);
				
					convertFloatToFloat16(A,temp);
				
					writeFile << B.Get_Exp() << ',' << B.Get_Mant();
				
					if (step < 19)
						writeFile << ',';
					else
						wtireFile << endl;
				
				}//end loop
				
				
		}
		//close write file
		//close rad file
		
	//}while(1)
	
	return(0);
};