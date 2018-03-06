import java.io.*;
import java.util.*;

/** This class executes the statements of program
 */
public class Main {
	static int alloca = 1;																	// allocation number counter
	static HashMap<String, Integer> variables = new HashMap<String, Integer>();				// stores variable names and their values
	static ArrayList<String> lines = new ArrayList<String>();								// all lines of the input file
	static String output = "";																// output string that is produced during arithmetic operation
	static int lineNo = 0;																	// number of line in output
	private static BufferedWriter bw;
	
	public static void main(String [] args) throws Exception{
		fileReading(args[0]);
		File fout = new File(args[1]);
		FileOutputStream fos = new FileOutputStream(fout);
	 
		bw = new BufferedWriter(new OutputStreamWriter(fos));
		bw.write(" ; ModuleID = \'stm2ir\'\ndeclare i32 @printf(i8*, ...)\n@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n\n");
		bw.write("define i32 @main() {\n");
		for(lineNo = 0 ; lineNo < lines.size() ; lineNo++){
			List<String> equation = stringtoRegex(lines.get(lineNo));		//take one line and split it
			if(equation.contains("=")){
				
				if(equation.indexOf("=") != equation.lastIndexOf("=")){	//there is more than one equals sign
					System.out.println("Error:\tLine " + (lineNo + 1) + ":\t More than one = sign");
					throw new Exception();
				}
				if(equation.indexOf("=") != 1){							// the equation is not assigned properly
					System.out.println("Error:\tLine " + (lineNo + 1) + ":\t Unwanted position of = sign");
					throw new Exception();
				}
				if(!equation.get(0).matches("[a-zA-Z][a-zA-Z0-9_]*")){
					System.out.println("Error:\tLine " + (lineNo + 1) + ":\t Illegal variable name");
					throw new Exception();
				}
				if(!variables.keySet().contains(equation.get(0))){
					bw.write("%" + equation.get(0) + " = alloca i32\n");
				}
				int RHS = evaluation(equation.subList(2, equation.size()));
				bw.write(output);
				output = "";
				//after calculations are done allocate the variable
				variables.put(equation.get(0), RHS);
				if(equation.subList(2, equation.size()).size() == 1){
					bw.write("store i32 " + RHS + ", i32* %"+ equation.get(0) +"\n");
				}
				else{
					bw.write("store i32 %" + (alloca-1) + ", i32* %"+ equation.get(0) +"\n");
				}
				
			}
			else{
				if(equation.size() == 1){
					bw.write("%" + alloca + " = load i32* %" + equation.get(0));
					bw.newLine();
					bw.write("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 %"+alloca+" )\n");
					alloca+=2;
				}
				else{
					int toPrint = evaluation(equation);
					bw.write(output);
					output = "";
					bw.newLine();
					bw.write("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 %"+(alloca-1)+" )\n");
					alloca++;
				}
			}
		
		}
		bw.write("ret i32 0\n}");
		bw.close();
	}
	
	/** File is read and stored inside lines List
	 * @param fileName name of file
	 */
	public static void fileReading(String args){
		File f = new File(args);
	    try {
	        Scanner sc = new Scanner(f);
	        while (sc.hasNextLine()) {
	            String str = sc.nextLine();
	            lines.add(str);
	        }
	        sc.close();
	    } 
	    catch (FileNotFoundException e) {
	    	System.out.println("File is not found");
	        e.printStackTrace();
	    }
	}
	
	/** Splits a string of arithmetic operation into tokens
	 * 	Example: 4 + 3 * (3 - 5) becomes ["4", "+", "3", "*", "(", "3", "-", "5", ")"]
	 * 	@param str	 String to split
	 * 	@return list of tokens
	 */
	public static List<String> stringtoRegex(String str){
		String exp = str;
		String[] tokens = exp.replaceAll("\\s*", "").split("(?<=[-+*/()=])|(?=[-+*/()=])");
		ArrayList<String> components = new ArrayList<String>(Arrays.asList(tokens));
		return components;
	}
	
	/**
	 * Evaluates a given arithmetic operation
	 * @param components	list of tokens of the operation
	 * @return				value of the operation
	 * @throws Exception	thrown if any illegal token is present
	 */
	public static int evaluation(List<String> components) throws Exception{
		List<Component> expressionList = new LinkedList<Component>();			// list of tokens rearranged and calculated according to precedence
		// decided what to do in the token in the given index
		for(int i = 0 ; i < components.size() ; i++){
			if(components.get(i).equals("(")){
				int matching = findMatching(components, i);
				if(matching == -1){		//if there is no closing parenthesis
					System.out.println("Error:\tLine " + (lineNo + 1) + ":\t Unmatching parenthesis ");
					throw new Exception();
				}
				int value = evaluation(components.subList(i+1, matching));		// value of arithmetic operation between parentheses
				expressionList.add(new Component(value, alloca - 1, true));
				i = matching;													// calculation is continuing from the closing parenthesis
				if(expressionList.size() > 2){
					int size = expressionList.size();
					if(expressionList.get(size-2).isOper && expressionList.get(size-2).oper.equals("*")){
						int newValue = expressionList.get(size - 3).num * expressionList.get(size - 1).num;
						output = output + "%" + alloca + " = mul i32 " + expressionList.get(size - 3).seenValue + ", " + expressionList.get(size - 1).seenValue + "\n";
						expressionList = expressionList.subList(0, size - 3);
						expressionList.add(new Component(newValue, alloca, true));
						alloca++;
					}
					else if(expressionList.get(size-2).isOper && expressionList.get(size-2).oper.equals("/")){
						int newValue = expressionList.get(size - 3).num / expressionList.get(size - 1).num;
						output = output + "%" + alloca + " = sdiv i32 " + expressionList.get(size - 3).seenValue + ", " + expressionList.get(size - 1).seenValue + "\n";
						expressionList = expressionList.subList(0, size - 3);
						expressionList.add(new Component(newValue, alloca, true));
						alloca++;
					}
				}
			}
			else if(components.get(i).equals("+") || components.get(i).equals("-") || components.get(i).equals("*") || components.get(i).equals("/")){
				expressionList.add(new Component(components.get(i)));
				if(components.size()>1 && components.get(i - 1).equals("+") || components.get(i - 1).equals("-") || components.get(i - 1).equals("*") || components.get(i - 1).equals("/")){
					System.out.println("Error:\tLine " + (lineNo + 1) + ":\t Two operators in a row ");
					throw new Exception();
				}
			}
			else if(isInteger(components.get(i))){
				expressionList.add(new Component(Integer.parseInt(components.get(i)), alloca));
				if(expressionList.size() > 2){
					int size = expressionList.size();
					if(expressionList.get(size-2).isOper && expressionList.get(size-2).oper.equals("*")){
						int newValue = expressionList.get(size - 3).num * expressionList.get(size - 1).num;
						output = output + "%" + alloca + " = mul i32 " + expressionList.get(size - 3).seenValue + ", " + expressionList.get(size - 1).seenValue + "\n";
						expressionList = expressionList.subList(0, size - 3);
						expressionList.add(new Component(newValue, alloca, true));
						alloca++;
					}
					else if(expressionList.get(size-2).isOper && expressionList.get(size-2).oper.equals("/")){
						int newValue = expressionList.get(size - 3).num / expressionList.get(size - 1).num;
						output = output + "%" + alloca + " = sdiv i32 " + expressionList.get(size - 3).seenValue + ", " + expressionList.get(size - 1).seenValue + "\n";
						expressionList = expressionList.subList(0, size - 3);
						expressionList.add(new Component(newValue, alloca, true));
						alloca++;
					}
				}
			}
			else if(variables.containsKey(components.get(i))){
				expressionList.add(new Component(components.get(i), variables.get(components.get(i)), alloca));
				output = output + "%" + alloca + " = load i32* %" + components.get(i) + "\n";
				alloca++;
				if(expressionList.size() > 2){
					int size = expressionList.size();
					if(expressionList.get(size-2).isOper && expressionList.get(size-2).oper.equals("*")){
						int newValue = expressionList.get(size - 3).num * expressionList.get(size - 1).num;
						output = output + "%" + alloca + " = mul i32 " + expressionList.get(size - 3).seenValue + ", " + expressionList.get(size - 1).seenValue + "\n";
						expressionList = expressionList.subList(0, size - 3);
						expressionList.add(new Component(newValue, alloca, true));
						alloca++;
					}
					else if(expressionList.get(size-2).isOper && expressionList.get(size-2).oper.equals("/")){
						int newValue = expressionList.get(size - 3).num / expressionList.get(size - 1).num;
						output = output + "%" + alloca + " = sdiv i32 " + expressionList.get(size - 3).seenValue + ", " + expressionList.get(size - 1).seenValue + "\n";
						expressionList = expressionList.subList(0, size - 3);
						expressionList.add(new Component(newValue, alloca, true));
						alloca++;
					}
				}
			
			}
			else{
				System.out.println("Error:\tLine " + (lineNo + 1) + ":\t Undefined token " + components.get(i));
				throw new Exception();
			}
		}
		// now, there is only arithmetic operation with + and - left
		// operation can be done respectively
		while(expressionList.size()>1){
			if(!expressionList.get(0).isNum && !expressionList.get(0).isVar){
				System.out.println("Error:\tLine " + (lineNo + 1) + ":\tTwo operators in a row ");
				throw new Exception();
			}
			if(!expressionList.get(0).isNum && !expressionList.get(0).isVar){
				System.out.println("Error:\tLine " + (lineNo + 1) + ":\tTwo operators in a row ");
				throw new Exception();
			}
			if(expressionList.get(1).oper.equals("+")){
				int newValue = expressionList.get(0).num + expressionList.get(2).num;
				output = output + "%" + alloca + " = add i32 " + expressionList.get(0).seenValue + ", " + expressionList.get(2).seenValue + "\n";
				expressionList.remove(0);
				expressionList.remove(0);
				expressionList.remove(0);
				expressionList.add(0, new Component(newValue, alloca, true));
				alloca++;
			}
			else if(expressionList.get(1).oper.equals("-")){
				int newValue = expressionList.get(0).num - expressionList.get(2).num;
				output = output + "%" + alloca + " = sub i32 " + expressionList.get(0).seenValue + ", " + expressionList.get(2).seenValue + "\n";
				expressionList.remove(0);
				expressionList.remove(0);
				expressionList.remove(0);
				expressionList.add(0, new Component(newValue, alloca, true));
				alloca++;
			}
			else{
				System.out.println("Error:\tLine " + (lineNo + 1) + ":\tIllegal operator ");
				throw new Exception();
			}
		}
		return expressionList.get(0).num;
	}
	
	/**
	 * Finds the matching closing parenthesis of a given opening parenthesis
	 * @param text	arithmetic operation
	 * @param index	index of opening parenthesis in the text
	 * @return	index of matching parenthesis, -1 is there is none
	 */
	public static int findMatching(List<String> text, int index) {
		int i = 1;
	    int matching = index;
	    while (i > 0) {
	    	matching++;
	        String c = text.get(matching);
	        if (c.equals("(")) {
	            i++;
	        }
	        else if (c.equals(")")){
	            i--;
	        }
	    }
	    return matching;
	}
	
	/**
	 * Finds whether given string is an integer or not
	 * @param str	String that is assumed to be an integer
	 * @return	true, if the given string can be parsed into integer
	 */
	public static boolean isInteger(String str){
	    try{
	        Integer.parseInt(str);
	        return true;
	    } 
	    catch (Exception e){
	        return false;
	    }
	}
}