#include <iostream>

using namespace std;

class float16{
	private:
		//Split the 16 bit value into two separate bytes
		uint8_t valExp; //One for the exponent
		uint8_t valMant; //One for the mantisa (implicit 2^0, and MSB is the sign bit of the final variable)
		//Keep a running Sum total for the addition of the 16 bit value
		uint8_t sumExp; //Also has an exponent byte
		uint8_t sumMant; //And a mantissa byte (same construction as of the final varibale)
	public:
		float16();	//Construction of the variable
		void Clear_Sum();	//A function to clear the running sum
		void Set_Exp(uint8_t);
		void Set_Mant(uint8_t);
		uint8_t Get_Exp();
		uint8_t Get_Mant();
		void Add_Float16(float16*); //Where the final variable is added to a temporary value of the same construction
};

float16::float16()
{
	//Constructor of type float16
	
	valExp = 0; //Clear the exponent byte
	valMant = 0; //Clear the matissa byte
	Clear_Sum(); //Call the Clear_Sum function.
}

void float16::Clear_Sum()
{
	sumExp = 0;
	sumExp = 0;
}	

void float16::Set_Exp(uint8_t tExp)	{valExp = tExp;}

void float16::Set_Mant(uint8_t tMant)	{valMant  = tMant;}

uint8_t float16::Get_Exp()	{return(valExp);}

uint8_t float16::Get_Mant()	{return(valMant);}

void float16::Add_Float16(float16* Temp)
{
	//Remove both values'sign bits and incorporate the implicit 2^0 bit
	uint8_t Fixed_A;
	uint8_t Fixed_B;
	unsigned int Overflow_check = 0;
	
	std::cout << "Masking mantissa:" << endl;
	Fixed_A = valMant&0x7F;//Mask the used bits
	Fixed_B = Temp->Get_Mant()&0x7F;
	
	Fixed_A = Fixed_A | 0x80;
	cout << "Old mantissa1: " << static_cast<int>(valMant) << " New mantissa1: " << static_cast<int>(Fixed_A) << endl;//Debug
	Fixed_B = Fixed_B | 0x80;
	cout << "Old mantissa2: " << static_cast<int>(Temp->Get_Mant()) << " New mantissa2: " << static_cast<int>(Fixed_B) << endl;//Debug
	
	
	//compare exp values
	std::cout << "Compare the exponents of the two variables." << endl;
	int Count = 1;
	while (Temp->Get_Exp() != valExp)
	{
		std::cout << "Loop count: " << Count << endl;
		if (Temp->Get_Exp() <= valExp)
		{
			std::cout << "Variable1 exponent is larger" << endl;
			Temp->Set_Exp(Temp->Get_Exp()+1);//Increment exponent value
			Fixed_B = Fixed_B >> 1; //Bit shift mantissa
			std::cout << "Increased old exponent2 to: " << static_cast<int>(Temp->Get_Exp()) << " Shifted old matissa2 to: " << static_cast<int>(Fixed_B) << endl;//Debug
		}
		else
		{
			std::cout << "Variable2 exponent is larger" << endl;
			valExp += 1;//Increment exponent value
			Fixed_A = Fixed_A >> 1;//Bit shift mantissa
			std::cout << "Increased old exponent to: " << static_cast<int>(valExp) << " Shifted old matissa to: " << static_cast<int>(Fixed_A) << endl;//Debug
		}
		Count += 1;
	}
	
	cout << "Both exponents are equal" << endl;//Debug
	//add mantissas
	std::cout << "Temp Val1 exponent: " << static_cast<int>(valExp) << " Temp Val1 matissa:" << static_cast<int>(Fixed_A) << endl;//Debug
	std::cout << "Temp Val2 exponent: " << static_cast<int>(Temp->Get_Exp()) << " Temp Val2 matissa: " << static_cast<int>(Fixed_B) << endl;//Debug
	Overflow_check = Fixed_A+Fixed_B;
	Fixed_A += Fixed_B;
	std::cout << "New Temp Val1 matissa: " << static_cast<int>(Fixed_A) << endl;//Debug
	
	//check overflow
	if (Overflow_check > 255)
	{
		std::cout << "Mantissa overflowed" << endl;
		//Add 1 to exponent and shift mantissa right
		valExp += 1;
		Fixed_A = Fixed_A >> 1;
		std::cout << "Temp Val1 exponent: " << static_cast<int>(valExp) << " Temp Val1 matissa: " << static_cast<int>(Fixed_A) << endl;//Debug
	}
	
	//Mask off implicit 2^0, and re-incorporate the sign bit
	std::cout << "Masking final mantissa and placing sign bit" << endl;
	Fixed_A = Fixed_A & 0x7F;
	std::cout << "Temp Val1 matissa: " << static_cast<int>(Fixed_A) << endl;//Debug
	valMant = Fixed_A;
}

int main ()
{
	float16 Var;
	float16 Var2;

	Var.Set_Exp(5);
	Var.Set_Mant(0xB0);
	std::cout << sizeof(Var) << " bytes used" << endl;
	std::cout << "Exponent1: " << static_cast<int>(Var.Get_Exp()) << " Mantissa1: " << static_cast<int>(Var.Get_Mant()) << endl;//Debug
	
	Var2.Set_Exp(6);
	Var2.Set_Mant(0xA0);
	std::cout << "Exponent2: " << static_cast<int>(Var2.Get_Exp()) << " Mantissa2: " << static_cast<int>(Var2.Get_Mant()) << endl;//Debug

	Var.Add_Float16(&Var2);
	
	std::cout << "Sum total is:" << endl;
	std::cout << "Exponent1: " << static_cast<int>(Var.Get_Exp()) << " Mantissa1: " << static_cast<int>(Var.Get_Mant()) << endl;//Debug

	return(0);
};