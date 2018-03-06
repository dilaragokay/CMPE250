/**
 * Instances of this class represents each token in an arithmetic expression 
 * Example: "6", "+", "-", "/", "*", "var1", ")"
 * @author 2014400102
 */
public class Component {
	String oper;						//Is an operator(+,-,*,/) if the instance is an operator
	boolean isOper;						//true, if instance is an operator
	boolean isCalculated;				//true, if instance is the result of a binary operation in the arithmetic operation
	boolean isVar;						//true, if instance is a variable
	boolean isNum;						//true, if instance is a number
	String var;							//Name of the variable
	int num;							//Number
	int alloc;							//Allocation number
	String seenValue;					//If isCalculated is true seenValue is the allocation number, else seenValue is the number
	/**
	 * Variable instantiation
	 * @param var	Name of variable
	 * @param num	Number
	 * @param alloc	Allocation number
	 */
	public Component(String var, int num, int alloc){
		this.alloc = alloc;
		isNum = false;
		isVar = true;
		isOper = false;
		oper = "";
		this.var = var;
		this.num = num;
		seenValue = "%"+alloc;
	}
	/**
	 * Number instantiation
	 * @param num	Number
	 * @param alloc	Allocaiton number
	 */
	public Component(int num, int alloc){
		isCalculated = false;
		isNum = true;
		isVar = false;
		isOper = false;
		oper = "";
		var = "";
		this.num = num;
		this.alloc = alloc;
		seenValue = ""+num;
	}
	/**
	 * Number instantiation 
	 * @param num	Number
	 * @param alloc	Allocation number
	 * @param isCalculated	true, if the allocation number of instance is used during calculation instead of the value of it
	 */
	public Component(int num, int alloc, boolean isCalculated){
		isCalculated = true;
		isNum = true;
		isVar = false;
		isOper = false;
		oper = "";
		var = "";
		this.num = num;
		this.alloc = alloc;
		seenValue = "%"+alloc;
	}
	/**
	 * Operator instantiation
	 * @param oper Operator
	 */
	public Component(String oper){
		isNum = false;
		this.oper = oper;
		isOper = true;
		isVar = false;
		var = "";
		num = -1;
	}
	
}